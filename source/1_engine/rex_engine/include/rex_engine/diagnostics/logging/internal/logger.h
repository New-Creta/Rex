

#pragma once

// Thread safe logger (except for set_error_handler())
// Has name, log level, vector of rsl::shared sink pointers and formatter
// Upon each log write the logger:
// 1. Checks if its log level is enough to log the message and if yes:
// 2. Call the underlying sinks to do the job.
// 3. Each sink use its own private copy of a formatter to format the message
// and send to its destination.
//
// The use of private formatter per sink provides the opportunity to cache some
// formatted data, and support for different format per sink.

#include "rex_std/format.h"

#include "rex_engine/types.h"
#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/backtracer.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"

#define REXLOG_LOGGER_CATCH(location)

namespace rexlog
{

  class REXLOG_API logger
  {
  public:
    // Empty logger
    explicit logger(rex::DebugString name)
        : name_(rsl::move(name))
        , sinks_()
    {
    }

    // Logger with range on sinks
    template <typename It>
    logger(rex::DebugString name, It begin, It end)
        : name_(rsl::move(name))
        , sinks_(begin, end)
    {
    }

    // Logger with single sink
    logger(rex::DebugString name, sink_ptr single_sink)
        : logger(rsl::move(name), {rsl::move(single_sink)})
    {
    }

    // Logger with sinks init list
    logger(rex::DebugString name, sinks_init_list sinks)
        : logger(rsl::move(name), sinks.begin(), sinks.end())
    {
    }

    virtual ~logger() = default;

    logger(const logger& other);
    logger(logger&& other) REXLOG_NOEXCEPT;
    logger& operator=(logger other) REXLOG_NOEXCEPT;
    void swap(rexlog::logger& other) REXLOG_NOEXCEPT;

    template <typename... Args>
    void log(source_loc loc, level::level_enum lvl, format_string_t<Args...> fmt, Args&&... args)
    {
      log_(loc, lvl, details::to_string_view(fmt), rsl::forward<Args>(args)...);
    }

    template <typename... Args>
    void log(level::level_enum lvl, format_string_t<Args...> fmt, Args&&... args)
    {
      log(source_loc {}, lvl, fmt, rsl::forward<Args>(args)...);
    }

    template <typename T>
    void log(level::level_enum lvl, const T& msg)
    {
      log(source_loc {}, lvl, msg);
    }

    // T cannot be statically converted to format string (including string_view/wstring_view)
    template <class T, typename rsl::enable_if<!is_convertible_to_any_format_string<const T&>::value, int>::type = 0>
    void log(source_loc loc, level::level_enum lvl, const T& msg)
    {
      log(loc, lvl, "{}", msg);
    }

    void log(log_clock::time_point log_time, source_loc loc, level::level_enum lvl, string_view_t msg)
    {
      bool log_enabled       = should_log(lvl);
      bool traceback_enabled = tracer_.enabled();
      if(!log_enabled && !traceback_enabled)
      {
        return;
      }

      details::log_msg log_msg(log_time, loc, name_, lvl, msg);
      log_it_(log_msg, log_enabled, traceback_enabled);
    }

    void log(source_loc loc, level::level_enum lvl, string_view_t msg)
    {
      bool log_enabled       = should_log(lvl);
      bool traceback_enabled = tracer_.enabled();
      if(!log_enabled && !traceback_enabled)
      {
        return;
      }

      details::log_msg log_msg(loc, name_, lvl, msg);
      log_it_(log_msg, log_enabled, traceback_enabled);
    }

    void log(level::level_enum lvl, string_view_t msg)
    {
      log(source_loc {}, lvl, msg);
    }

    template <typename... Args>
    void trace(format_string_t<Args...> fmt, Args&&... args)
    {
      log(level::trace, fmt, rsl::forward<Args>(args)...);
    }

    template <typename... Args>
    void debug(format_string_t<Args...> fmt, Args&&... args)
    {
      log(level::debug, fmt, rsl::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(format_string_t<Args...> fmt, Args&&... args)
    {
      log(level::info, fmt, rsl::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(format_string_t<Args...> fmt, Args&&... args)
    {
      log(level::warn, fmt, rsl::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(format_string_t<Args...> fmt, Args&&... args)
    {
      log(level::err, fmt, rsl::forward<Args>(args)...);
    }

    template <typename... Args>
    void critical(format_string_t<Args...> fmt, Args&&... args)
    {
      log(level::critical, fmt, rsl::forward<Args>(args)...);
    }

    template <typename T>
    void trace(const T& msg)
    {
      log(level::trace, msg);
    }

    template <typename T>
    void debug(const T& msg)
    {
      log(level::debug, msg);
    }

    template <typename T>
    void info(const T& msg)
    {
      log(level::info, msg);
    }

    template <typename T>
    void warn(const T& msg)
    {
      log(level::warn, msg);
    }

    template <typename T>
    void error(const T& msg)
    {
      log(level::err, msg);
    }

    template <typename T>
    void critical(const T& msg)
    {
      log(level::critical, msg);
    }

    // return true logging is enabled for the given level.
    bool should_log(level::level_enum msg_level) const
    {
      return msg_level >= level_.load(rsl::memory_order_relaxed);
    }

    // return true if backtrace logging is enabled.
    bool should_backtrace() const
    {
      return tracer_.enabled();
    }

    void set_level(level::level_enum log_level);

    level::level_enum level() const;

    const rex::DebugString& name() const;

    // set formatting for the sinks in this logger.
    // each sink will get a separate instance of the formatter object.
    void set_formatter(rsl::unique_ptr<formatter> f);

    // set formatting for the sinks in this logger.
    // equivalent to
    //     set_formatter(make_unique<pattern_formatter>(pattern, time_type))
    // Note: each sink will get a new instance of a formatter object, replacing the old one.
    void set_pattern(rex::DebugString pattern, pattern_time_type time_type = pattern_time_type::local);

    // backtrace support.
    // efficiently store all debug/trace messages in a circular buffer until needed for debugging.
    void enable_backtrace(size_t n_messages);
    void disable_backtrace();
    void dump_backtrace();

    // flush functions
    void flush();
    void flush_on(level::level_enum log_level);
    level::level_enum flush_level() const;

    // sinks
    const rex::DebugVector<sink_ptr>& sinks() const;

    rex::DebugVector<sink_ptr>& sinks();

    // error handler
    void set_error_handler(err_handler);

    // create new logger with same sinks and configuration.
    virtual rsl::shared_ptr<logger> clone(rex::DebugString logger_name);

  protected:
    rex::DebugString name_;
    rex::DebugVector<sink_ptr> sinks_;
    rexlog::level_t level_ {level::info};
    rexlog::level_t flush_level_ {level::off};
    err_handler custom_err_handler_ {nullptr};
    details::backtracer tracer_;

    // common implementation for after templated public api has been resolved
    template <typename... Args>
    void log_(source_loc loc, level::level_enum lvl, string_view_t fmt, Args&&... args)
    {
      bool log_enabled       = should_log(lvl);
      bool traceback_enabled = tracer_.enabled();
      if(!log_enabled && !traceback_enabled)
      {
        return;
      }
      REXLOG_TRY
      {
        memory_buf_t buf;
        fmt_lib::vformat_to(rsl::back_inserter(buf), fmt, fmt_lib::make_format_args(rsl::forward<Args>(args)...));

        details::log_msg log_msg(loc, name_, lvl, string_view_t(buf.data(), buf.size()));
        log_it_(log_msg, log_enabled, traceback_enabled);
      }
      REXLOG_LOGGER_CATCH(loc)
    }

    // log the given message (if the given log level is high enough),
    // and save backtrace (if backtrace is enabled).
    void log_it_(const details::log_msg& log_msg, bool log_enabled, bool traceback_enabled);
    virtual void sink_it_(const details::log_msg& msg);
    virtual void flush_();
    void dump_backtrace_();
    bool should_flush_(const details::log_msg& msg);

    // handle errors during logging.
    // default handler prints the error to stderr at max rate of 1 message/sec.
    void err_handler_(const rex::DebugString& msg);
  };

  void swap(logger& a, logger& b);

} // namespace rexlog
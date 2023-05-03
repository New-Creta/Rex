

#pragma once

// Thread safe Logger (except for set_error_handler())
// Has name, log level, vector of rsl::shared sink pointers and formatter
// Upon each log write the Logger:
// 1. Checks if its log level is enough to log the message and if yes:
// 2. Call the underlying sinks to do the job.
// 3. Each sink use its own private copy of a formatter to format the message
// and send to its destination.
//
// The use of private formatter per sink provides the opportunity to cache some
// formatted data, and support for different format per sink.

#include "rex_engine/debug_types.h"
#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/backtracer.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_std/atomic.h"
#include "rex_std/format.h"

#define REXLOG_LOGGER_CATCH(location)

namespace rexlog
{

  class REXLOG_API Logger
  {
  public:
    // Empty Logger
    explicit Logger(rex::DebugString name)
        : m_name(rsl::move(name))
        , m_sinks()
    {
    }

    // Logger with range on sinks
    template <typename It>
    Logger(rex::DebugString name, It begin, It end)
        : m_name(rsl::move(name))
        , m_sinks(begin, end)
    {
    }

    // Logger with single sink
    Logger(rex::DebugString name, sink_ptr singleSink)
        : Logger(rsl::move(name), {rsl::move(singleSink)})
    {
    }

    // Logger with sinks init list
    Logger(rex::DebugString name, sinks_init_list sinks)
        : Logger(rsl::move(name), sinks.begin(), sinks.end())
    {
    }

    virtual ~Logger() = default;

    Logger(const Logger& other);
    Logger(Logger&& other) REXLOG_NOEXCEPT;
    Logger& operator=(const Logger& other) REXLOG_NOEXCEPT;
    Logger& operator=(Logger&& other) REXLOG_NOEXCEPT;
    void swap(rexlog::Logger& other) REXLOG_NOEXCEPT;

    template <typename... Args>
    void log(SourceLoc loc, level::LevelEnum lvl, format_string_t<Args...> fmt, Args&&... args)
    {
      log_impl(loc, lvl, details::to_string_view(fmt), rsl::forward<Args>(args)...);
    }

    template <typename... Args>
    void log(level::LevelEnum lvl, format_string_t<Args...> fmt, Args&&... args)
    {
      log(SourceLoc {}, lvl, fmt, rsl::forward<Args>(args)...);
    }

    template <typename T>
    void log(level::LevelEnum lvl, const T& msg)
    {
      log(SourceLoc {}, lvl, msg);
    }

    // T cannot be statically converted to format string (including string_view/wstring_view)
    template <class T, typename rsl::enable_if<!IsConvertibleToAnyFormatString<const T&>::value, int>::type = 0>
    void log(SourceLoc loc, level::LevelEnum lvl, const T& msg)
    {
      log(loc, lvl, "{}", msg);
    }

    void log(log_clock::time_point logTime, SourceLoc loc, level::LevelEnum lvl, string_view_t msg)
    {
      const bool log_enabled       = should_log(lvl);
      const bool traceback_enabled = m_tracer.enabled();
      if(!log_enabled && !traceback_enabled)
      {
        return;
      }

      details::LogMsg const log_msg(logTime, loc, m_name, lvl, msg);
      log_it_impl(log_msg, log_enabled, traceback_enabled);
    }

    void log(SourceLoc loc, level::LevelEnum lvl, string_view_t msg)
    {
      const bool log_enabled       = should_log(lvl);
      const bool traceback_enabled = m_tracer.enabled();
      if(!log_enabled && !traceback_enabled)
      {
        return;
      }

      details::LogMsg const log_msg(loc, m_name, lvl, msg);
      log_it_impl(log_msg, log_enabled, traceback_enabled);
    }

    void log(level::LevelEnum lvl, string_view_t msg)
    {
      log(SourceLoc {}, lvl, msg);
    }

    template <typename... Args>
    void trace(format_string_t<Args...> fmt, Args&&... args)
    {
      log(level::Trace, fmt, rsl::forward<Args>(args)...);
    }

    template <typename... Args>
    void debug(format_string_t<Args...> fmt, Args&&... args)
    {
      log(level::Debug, fmt, rsl::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(format_string_t<Args...> fmt, Args&&... args)
    {
      log(level::Info, fmt, rsl::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(format_string_t<Args...> fmt, Args&&... args)
    {
      log(level::Warn, fmt, rsl::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(format_string_t<Args...> fmt, Args&&... args)
    {
      log(level::Err, fmt, rsl::forward<Args>(args)...);
    }

    template <typename... Args>
    void critical(format_string_t<Args...> fmt, Args&&... args)
    {
      log(level::Critical, fmt, rsl::forward<Args>(args)...);
    }

    template <typename T>
    void trace(const T& msg)
    {
      log(level::Trace, msg);
    }

    template <typename T>
    void debug(const T& msg)
    {
      log(level::Debug, msg);
    }

    template <typename T>
    void info(const T& msg)
    {
      log(level::Info, msg);
    }

    template <typename T>
    void warn(const T& msg)
    {
      log(level::Warn, msg);
    }

    template <typename T>
    void error(const T& msg)
    {
      log(level::Err, msg);
    }

    template <typename T>
    void critical(const T& msg)
    {
      log(level::Critical, msg);
    }

    // return true logging is enabled for the given level.
    bool should_log(level::LevelEnum msgLevel) const
    {
      return msgLevel >= m_level.load(rsl::memory_order_relaxed);
    }

    // return true if backtrace logging is enabled.
    bool should_backtrace() const
    {
      return m_tracer.enabled();
    }

    void set_level(level::LevelEnum logLevel);

    level::LevelEnum level() const;

    const rex::DebugString& name() const;

    // set formatting for the sinks in this Logger.
    // each sink will get a separate instance of the formatter object.
    void set_formatter(rsl::unique_ptr<formatter> f);

    // set formatting for the sinks in this Logger.
    // equivalent to
    //     set_formatter(make_unique<pattern_formatter>(pattern, time_type))
    // Note: each sink will get a new instance of a formatter object, replacing the old one.
    void set_pattern(rex::DebugString pattern, PatternTimeType timeType = PatternTimeType::Local);

    // backtrace support.
    // efficiently store all debug/trace messages in a circular buffer until needed for debugging.
    void enable_backtrace(size_t nMessages);
    void disable_backtrace();
    void dump_backtrace();

    // flush functions
    void flush();
    void flush_on(level::LevelEnum logLevel);
    level::LevelEnum flush_level() const;

    // sinks
    const rex::DebugVector<sink_ptr>& sinks() const;

    rex::DebugVector<sink_ptr>& sinks();

    // error handler
    void set_error_handler(err_handler /*handler*/);

    // create new Logger with same sinks and configuration.
    virtual rsl::shared_ptr<Logger> clone(rex::DebugString loggerName);

  protected:
    // common implementation for after templated public api has been resolved
    template <typename... Args>
    void log_impl(SourceLoc loc, level::LevelEnum lvl, string_view_t fmt, Args&&... args)
    {
      const bool log_enabled       = should_log(lvl);
      const bool traceback_enabled = m_tracer.enabled();
      if(!log_enabled && !traceback_enabled)
      {
        return;
      }
      REXLOG_TRY
      {
        memory_buf_t buf;
        fmt_lib::vformat_to(rsl::back_inserter(buf), fmt, fmt_lib::make_format_args(rsl::forward<Args>(args)...));

        const details::LogMsg log_msg(loc, m_name, lvl, string_view_t(buf.data(), buf.size()));
        log_it_impl(log_msg, log_enabled, traceback_enabled);
      }
      REXLOG_LOGGER_CATCH(loc)
    }

    // log the given message (if the given log level is high enough),
    // and save backtrace (if backtrace is enabled).
    void log_it_impl(const details::LogMsg& logMsg, bool logEnabled, bool tracebackEnabled);
    virtual void sink_it_impl(const details::LogMsg& msg);
    virtual void flush_impl();
    void dump_backtrace_impl();
    bool should_flush_impl(const details::LogMsg& msg);

    // handle errors during logging.
    // default handler prints the error to stderr at max rate of 1 message/sec.
    void err_handler_impl(const rex::DebugString& msg);

    void set_name(rex::DebugString&& name);

  private:
    rex::DebugString m_name;
    rex::DebugVector<sink_ptr> m_sinks;
    rexlog::level_t m_level {level::Info};
    rexlog::level_t m_flush_level {level::Off};
    err_handler m_custom_err_handler {nullptr};
    details::Backtracer m_tracer;
  };

  void swap(Logger& a, Logger& b);

} // namespace rexlog
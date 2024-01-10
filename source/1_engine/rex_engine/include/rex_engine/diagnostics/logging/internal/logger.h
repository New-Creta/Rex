

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

#include "rex_engine/engine/debug_types.h"
#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/string.h"
#include "rex_std/format.h"
#include "rex_std/source_location.h"

namespace rex
{
  namespace log
  {
    using Sinks = rex::DebugVector<sink_ptr>;

    class Logger
    {
    public:
      explicit Logger(rsl::string_view name);

      Logger(rsl::string_view name, sink_ptr singleSink);
      Logger(rsl::string_view name, sinks_init_list sinks);

      //-------------------------------------------------------------------------
      template <typename It>
      Logger(rsl::string_view name, It begin, It end)
          : m_name(rex::DebugString(name))
          , m_sinks(begin, end)
      {
      }

      virtual ~Logger() = default;

      Logger(const Logger& other);
      Logger(Logger&& other) noexcept;
      Logger& operator=(const Logger& other) noexcept;
      Logger& operator=(Logger&& other) noexcept;

    public:
      void set_level(level::LevelEnum logLevel);
      void set_formatter(PatternFormatter f);
      void set_pattern(rsl::string_view pattern, PatternTimeType timeType = PatternTimeType::Local);

      bool should_log(level::LevelEnum msgLevel) const;

      rsl::string_view name() const;

      level::LevelEnum level() const;
      level::LevelEnum flush_level() const;

      void flush();
      void flush_on(level::LevelEnum logLevel);

      const Sinks& sinks() const;
      Sinks& sinks();

      virtual rsl::shared_ptr<Logger> clone(rsl::string_view loggerName);

      void swap(rex::log::Logger& other) noexcept;

    public:
      void log(log_clock::time_point logTime, rsl::source_location loc, level::LevelEnum lvl, rsl::string_view msg);
      void log(rsl::source_location loc, level::LevelEnum lvl, rsl::string_view msg);
      void log(level::LevelEnum lvl, rsl::string_view msg);

      //-------------------------------------------------------------------------
      // T cannot be statically converted to format string (including string_view/wstring_view)
      template <class T, typename rsl::enable_if<!IsConvertibleToAnyFormatString<const T&>::value, int>::type = 0>
      void log(rsl::source_location loc, level::LevelEnum lvl, const T& msg)
      {
        log(loc, lvl, "{}", msg);
      }
      //-------------------------------------------------------------------------
      template <typename... Args>
      void log(rsl::source_location loc, level::LevelEnum lvl, format_string_t<Args...> fmt, Args&&... args)
      {
        log_impl(loc, lvl, details::to_string_view(fmt), rsl::forward<Args>(args)...);
      }
      //-------------------------------------------------------------------------
      template <typename... Args>
      void log(level::LevelEnum lvl, format_string_t<Args...> fmt, Args&&... args)
      {
        log(rsl::source_location {}, lvl, fmt, rsl::forward<Args>(args)...);
      }
      //-------------------------------------------------------------------------
      template <typename T>
      void log(level::LevelEnum lvl, const T& msg)
      {
        log(rsl::source_location {}, lvl, msg);
      }

    protected:
      //-------------------------------------------------------------------------
      // common implementation for after templated public api has been resolved
      template <typename... Args>
      void log_impl(rsl::source_location loc, level::LevelEnum lvl, rsl::string_view fmt, Args&&... args)
      {
        const bool log_enabled = should_log(lvl);
        if(!log_enabled)
        {
          return;
        }

        rsl::big_stack_string buf;
        rsl::vformat_to(rsl::back_inserter(buf), fmt, rsl::make_format_args(rsl::forward<Args>(args)...));

        const details::LogMsg log_msg(loc, m_name, lvl, rsl::string_view(buf.data(), buf.size()));
        log_it_impl(log_msg, log_enabled);
      }

    protected:
      bool should_flush_impl(const details::LogMsg& msg);

      void log_it_impl(const details::LogMsg& logMsg, bool logEnabled);
      virtual void sink_it_impl(const details::LogMsg& msg);
      virtual void flush_it_impl();

      void set_name(rsl::string_view name);

    private:
      rex::DebugString m_name;
      Sinks m_sinks;
      rex::log::level_t m_level {static_cast<s32>(level::LevelEnum::Info)};
      rex::log::level_t m_flush_level {static_cast<s32>(level::LevelEnum::Off)};
    };

    void swap(Logger& a, Logger& b);
  } // namespace log
} // namespace rex
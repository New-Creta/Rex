#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"

#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_std/chrono.h"

// IWYU pragma: no_include <built-in>

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rex
{
  namespace log
  {
    namespace details
    {
      //-------------------------------------------------------------------------
      LogMsg::LogMsg()
          : m_logger_name()
          , m_level(level::LevelEnum::Off)
          , m_time()
          , m_thread_id(0)
          , m_color_range_start(0)
          , m_color_range_end(0)
          , m_source()
          , m_payload()
      {
      }

      //-------------------------------------------------------------------------
      LogMsg::LogMsg(rex::log::log_clock::time_point logTime, const rsl::source_location& loc, rsl::string_view aLoggerName, rex::log::level::LevelEnum lvl, rsl::string_view msg)
          : m_logger_name(aLoggerName)
          , m_level(lvl)
          , m_time(logTime)
          , m_thread_id(os::thread_id())
          , m_color_range_start(0)
          , m_color_range_end(0)
          , m_source(loc)
          , m_payload(msg)
      {
      }

      //-------------------------------------------------------------------------
      LogMsg::LogMsg(const rsl::source_location& loc, rsl::string_view aLoggerName, rex::log::level::LevelEnum lvl, rsl::string_view msg)
          : LogMsg(os::now(), loc, aLoggerName, lvl, msg)
      {
      }

      //-------------------------------------------------------------------------
      LogMsg::LogMsg(rsl::string_view aLoggerName, rex::log::level::LevelEnum lvl, rsl::string_view msg)
          : LogMsg(os::now(), rsl::source_location {}, aLoggerName, lvl, msg)
      {
      }

      //-------------------------------------------------------------------------
      void LogMsg::set_logger_name(rsl::string_view loggerName)
      {
        m_logger_name = loggerName;
      }
      //-------------------------------------------------------------------------
      void LogMsg::set_level(level::LevelEnum level)
      {
        m_level = level;
      }
      //-------------------------------------------------------------------------
      void LogMsg::set_time(log_clock::time_point tp)
      {
        m_time = tp;
      }
      //-------------------------------------------------------------------------
      void LogMsg::set_thread_id(size_t id)
      {
        m_thread_id = id;
      }

      //-------------------------------------------------------------------------
      void LogMsg::set_color_range_start(s32 cs)
      {
        m_color_range_start = cs;
      }
      //-------------------------------------------------------------------------
      void LogMsg::set_color_range_end(s32 ce)
      {
        m_color_range_end = ce;
      }

      //-------------------------------------------------------------------------
      void LogMsg::set_source(const rsl::source_location& sourceLoc)
      {
        m_source = sourceLoc;
      }
      //-------------------------------------------------------------------------
      void LogMsg::set_payload(rsl::string_view p)
      {
        m_payload = p;
      }

      //-------------------------------------------------------------------------
      rsl::string_view LogMsg::logger_name() const
      {
        return m_logger_name;
      }
      //-------------------------------------------------------------------------
      level::LevelEnum LogMsg::level() const
      {
        return m_level;
      }
      //-------------------------------------------------------------------------
      log_clock::time_point LogMsg::time() const
      {
        return m_time;
      }
      //-------------------------------------------------------------------------
      size_t LogMsg::thread_id() const
      {
        return m_thread_id;
      }

      //-------------------------------------------------------------------------
      s32 LogMsg::color_range_start() const
      {
        return m_color_range_start;
      }
      //-------------------------------------------------------------------------
      s32 LogMsg::color_range_end() const
      {
        return m_color_range_end;
      }

      //-------------------------------------------------------------------------
      const rsl::source_location& LogMsg::source() const
      {
        return m_source;
      }
      //-------------------------------------------------------------------------
      rsl::string_view LogMsg::payload() const
      {
        return m_payload;
      }
    } // namespace details
  }   // namespace log
} // namespace rex

// NOLINTEND(misc-definitions-in-headers)

#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
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
        LogMsg::LogMsg(rexlog::log_clock::time_point logTime, rexlog::SourceLoc loc, string_view_t aLoggerName, rexlog::level::LevelEnum lvl, rexlog::string_view_t msg)
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
        LogMsg::LogMsg(rexlog::SourceLoc loc, string_view_t aLoggerName, rexlog::level::LevelEnum lvl, rexlog::string_view_t msg)
            : LogMsg(os::now(), loc, aLoggerName, lvl, msg)
        {
        }

        //-------------------------------------------------------------------------
        LogMsg::LogMsg(string_view_t aLoggerName, rexlog::level::LevelEnum lvl, rexlog::string_view_t msg)
            : LogMsg(os::now(), SourceLoc{}, aLoggerName, lvl, msg)
        {
        }

        //-------------------------------------------------------------------------
        void LogMsg::set_logger_name(string_view_t loggerName)
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
        void LogMsg::set_color_range_start(size_t cs)
        {
            m_color_range_start = cs;
        }
        //-------------------------------------------------------------------------
        void LogMsg::set_color_range_end(size_t ce)
        {
            m_color_range_end = ce;
        }

        //-------------------------------------------------------------------------
        void LogMsg::set_source(const SourceLoc& sourceLoc)
        {
            m_source = sourceLoc;
        }
        //-------------------------------------------------------------------------
        void LogMsg::set_payload(string_view_t p)
        {
            m_payload = p;
        }

        //-------------------------------------------------------------------------
        string_view_t LogMsg::logger_name() const
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
        size_t LogMsg::color_range_start() const
        {
            return m_color_range_start;
        }
        //-------------------------------------------------------------------------
        size_t LogMsg::color_range_end() const
        {
            return m_color_range_end;
        }

        //-------------------------------------------------------------------------
        SourceLoc LogMsg::source() const
        {
            return m_source;
        }
        //-------------------------------------------------------------------------
        string_view_t LogMsg::payload() const
        {
            return m_payload;
        }
    } // namespace details
} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)

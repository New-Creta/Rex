#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{
    namespace details
    {
        LogMsg::LogMsg(rexlog::log_clock::time_point logTime, rsl::source_location loc, string_view_t aLoggerName, rexlog::level::LevelEnum lvl, rexlog::string_view_t msg)
            : logger_name(aLoggerName)
            , level(lvl)
            , time(logTime)
            , thread_id(os::thread_id())
            , source(loc)
            , payload(msg)
        {
        }

        LogMsg::LogMsg(rsl::source_location loc, string_view_t aLoggerName, rexlog::level::LevelEnum lvl, rexlog::string_view_t msg)
            : LogMsg(os::now(), loc, aLoggerName, lvl, msg)
        {
        }

        LogMsg::LogMsg(string_view_t aLoggerName, rexlog::level::LevelEnum lvl, rexlog::string_view_t msg)
            : LogMsg(os::now(), rsl::source_location{}, aLoggerName, lvl, msg)
        {
        }

    } // namespace details
} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)

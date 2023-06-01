#include "rex_engine/diagnostics/logging/internal/common.h"

#include "rex_std/format.h"

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{
    namespace level
    {

        static rsl::string_view g_level_string_views[]
        {
            REXLOG_LEVEL_NAME_TRACE, REXLOG_LEVEL_NAME_DEBUG, REXLOG_LEVEL_NAME_INFO, REXLOG_LEVEL_NAME_WARNING, REXLOG_LEVEL_NAME_ERROR, REXLOG_LEVEL_NAME_CRITICAL, REXLOG_LEVEL_NAME_OFF                                                                  \
        };

        static rsl::string_view g_short_level_names[]
        {
            REXLOG_LEVEL_SNAME_TRACE, REXLOG_LEVEL_SNAME_DEBUG, REXLOG_LEVEL_SNAME_INFO, REXLOG_LEVEL_SNAME_WARNING, REXLOG_LEVEL_SNAME_ERROR, REXLOG_LEVEL_SNAME_CRITICAL, REXLOG_LEVEL_SNAME_OFF                                                           \
        };

        rsl::string_view to_string_view(rexlog::level::LevelEnum l) noexcept
        {
            switch (l)
            {
            case LevelEnum::Off: return REXLOG_LEVEL_NAME_OFF;
            case LevelEnum::Critical: return REXLOG_LEVEL_NAME_CRITICAL;
            case LevelEnum::Err: return REXLOG_LEVEL_NAME_ERROR;
            case LevelEnum::Warn: return REXLOG_LEVEL_NAME_WARNING;
            case LevelEnum::Info: return REXLOG_LEVEL_NAME_INFO;
            case LevelEnum::Debug: return REXLOG_LEVEL_NAME_DEBUG;
            case LevelEnum::Trace: return REXLOG_LEVEL_NAME_TRACE;
            default:
                printf("Invalid level::LevelEnum - No conversion possible");
                return "";
            }
        };

        rsl::string_view to_short_c_str(rexlog::level::LevelEnum l) noexcept
        {
            switch (l)
            {
            case LevelEnum::Off: return REXLOG_LEVEL_SNAME_OFF;
            case LevelEnum::Critical: return REXLOG_LEVEL_SNAME_CRITICAL;
            case LevelEnum::Err: return REXLOG_LEVEL_SNAME_ERROR;
            case LevelEnum::Warn: return REXLOG_LEVEL_SNAME_WARNING;
            case LevelEnum::Info: return REXLOG_LEVEL_SNAME_INFO;
            case LevelEnum::Debug: return REXLOG_LEVEL_SNAME_DEBUG;
            case LevelEnum::Trace: return REXLOG_LEVEL_SNAME_TRACE;
            default:
                printf("Invalid level::LevelEnum - No conversion possible");
                return "";
            }
        }

        rexlog::level::LevelEnum from_str(const rex::DebugString& name) noexcept
        {
            const auto* it = rsl::find(rsl::begin(g_level_string_views), rsl::end(g_level_string_views), name);
            if (it != rsl::end(g_level_string_views))
            {
                return static_cast<level::LevelEnum>(rsl::distance(rsl::cbegin(g_level_string_views), it));
            }

            // check also for "warn" and "err" before giving up..
            if (name == "warn")
            {
                return level::LevelEnum::Warn;
            }
            if (name == "err")
            {
                return level::LevelEnum::Err;
            }

            return level::LevelEnum::Off;
        }
    } // namespace level
} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)

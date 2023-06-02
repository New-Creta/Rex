#include "rex_engine/diagnostics/logging/internal/common.h"

#include "rex_std/format.h"

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{
  namespace level
  {

    static rsl::array<rsl::string_view, 7> g_level_string_views = {g_level_name_trace, g_level_name_debug, g_level_name_info, g_level_name_warning, g_level_name_error, g_level_name_critical, g_level_name_off};

    static rsl::array<rsl::string_view, 7> g_short_level_names[] {g_level_sname_trace, g_level_sname_debug, g_level_sname_info, g_level_sname_warning, g_level_sname_error, g_level_sname_critical, g_level_sname_off};

    rsl::string_view to_string_view(rexlog::level::LevelEnum l) noexcept
    {
      switch(l)
      {
        case LevelEnum::Off: return g_level_name_off;
        case LevelEnum::Critical: return g_level_name_critical;
        case LevelEnum::Err: return g_level_name_error;
        case LevelEnum::Warn: return g_level_name_warning;
        case LevelEnum::Info: return g_level_name_info;
        case LevelEnum::Debug: return g_level_name_debug;
        case LevelEnum::Trace: return g_level_name_trace;
        default: printf("Invalid level::LevelEnum - No conversion possible"); return "";
      }
    };

    rsl::string_view to_short_c_str(rexlog::level::LevelEnum l) noexcept
    {
      switch(l)
      {
        case LevelEnum::Off: return g_level_sname_trace;
        case LevelEnum::Critical: return g_level_sname_critical;
        case LevelEnum::Err: return g_level_sname_error;
        case LevelEnum::Warn: return g_level_sname_warning;
        case LevelEnum::Info: return g_level_sname_info;
        case LevelEnum::Debug: return g_level_sname_debug;
        case LevelEnum::Trace: return g_level_sname_trace;
        default: printf("Invalid level::LevelEnum - No conversion possible"); return "";
      }
    }

    rexlog::level::LevelEnum from_str(rsl::string_view name) noexcept
    {
      const auto it = rsl::find(rsl::cbegin(g_level_string_views), rsl::cend(g_level_string_views), name);
      if(it != rsl::end(g_level_string_views))
      {
        return static_cast<level::LevelEnum>(rsl::distance(rsl::cbegin(g_level_string_views), it));
      }

      // check also for "warn" and "err" before giving up..
      if(name == "warn")
      {
        return level::LevelEnum::Warn;
      }
      if(name == "err")
      {
        return level::LevelEnum::Err;
      }

      return level::LevelEnum::Off;
    }
  } // namespace level
} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)

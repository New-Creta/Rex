

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include <string>

namespace rexlog
{
  namespace details
  {
    struct REXLOG_API LogMsg
    {
      LogMsg() = default;
      LogMsg(log_clock::time_point log_time, source_loc loc, string_view_t logger_name, level::level_enum lvl, string_view_t msg);
      LogMsg(source_loc loc, string_view_t logger_name, level::level_enum lvl, string_view_t msg);
      LogMsg(string_view_t logger_name, level::level_enum lvl, string_view_t msg);
      LogMsg(const LogMsg& other)            = default;
      LogMsg& operator=(const LogMsg& other) = default;

      string_view_t logger_name;
      level::level_enum level {level::off};
      log_clock::time_point time;
      size_t thread_id {0};

      // wrapping the formatted text with color (updated by pattern_formatter).
      mutable size_t color_range_start {0};
      mutable size_t color_range_end {0};

      source_loc source;
      string_view_t payload;
    };
  } // namespace details
} // namespace rexlog


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
      LogMsg(log_clock::time_point logTime, SourceLoc loc, string_view_t loggerName, level::LevelEnum lvl, string_view_t msg);
      LogMsg(SourceLoc loc, string_view_t loggerName, level::LevelEnum lvl, string_view_t msg);
      LogMsg(string_view_t loggerName, level::LevelEnum lvl, string_view_t msg);

      string_view_t logger_name;
      level::LevelEnum level {level::Off};
      log_clock::time_point time;
      size_t thread_id {0};

      // wrapping the formatted text with color (updated by pattern_formatter).
      mutable size_t color_range_start {0};
      mutable size_t color_range_end {0};

      SourceLoc source;
      string_view_t payload;
    };
  } // namespace details
} // namespace rexlog
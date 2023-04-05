

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/logger.h"
#include "rex_engine/diagnostics/logging/internal/sinks/stdout_color_sinks.h"

namespace rexlog
{

  template <typename Factory>
  REXLOG_INLINE rsl::shared_ptr<logger> stdout_color_mt(const rex::DebugString& logger_name, color_mode mode)
  {
    return Factory::template create<sinks::stdout_color_sink_mt>(rex::DebugString(logger_name), mode);
  }

  template <typename Factory>
  REXLOG_INLINE rsl::shared_ptr<logger> stdout_color_st(const rex::DebugString& logger_name, color_mode mode)
  {
    return Factory::template create<sinks::stdout_color_sink_st>(rex::DebugString(logger_name), mode);
  }

  template <typename Factory>
  REXLOG_INLINE rsl::shared_ptr<logger> stderr_color_mt(const rex::DebugString& logger_name, color_mode mode)
  {
    return Factory::template create<sinks::stderr_color_sink_mt>(rex::DebugString(logger_name), mode);
  }

  template <typename Factory>
  REXLOG_INLINE rsl::shared_ptr<logger> stderr_color_st(const rex::DebugString& logger_name, color_mode mode)
  {
    return Factory::template create<sinks::stderr_color_sink_st>(rex::DebugString(logger_name), mode);
  }
} // namespace rexlog

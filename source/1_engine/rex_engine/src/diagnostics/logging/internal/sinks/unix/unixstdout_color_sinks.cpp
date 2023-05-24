#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/sinks/unix/unixstdout_color_sinks.h"

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{

  template <typename Factory>
   rsl::shared_ptr<Logger> stdout_color_mt(const rex::DebugString& loggerName, ColorMode mode)
  {
    return Factory::template create<sinks::stdout_color_sink_mt>(rex::DebugString(loggerName), mode);
  }

  template <typename Factory>
   rsl::shared_ptr<Logger> stdout_color_st(const rex::DebugString& loggerName, ColorMode mode)
  {
    return Factory::template create<sinks::stdout_color_sink_st>(rex::DebugString(loggerName), mode);
  }

  template <typename Factory>
   rsl::shared_ptr<Logger> stderr_color_mt(const rex::DebugString& loggerName, ColorMode mode)
  {
    return Factory::template create<sinks::stderr_color_sink_mt>(rex::DebugString(loggerName), mode);
  }

  template <typename Factory>
   rsl::shared_ptr<Logger> stderr_color_st(const rex::DebugString& loggerName, ColorMode mode)
  {
    return Factory::template create<sinks::stderr_color_sink_st>(rex::DebugString(loggerName), mode);
  }
} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)

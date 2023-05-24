#include "rex_engine/diagnostics/logging/internal/sinks/unix/unixstdout_sinks.h"

// NOLINTBEGIN(misc-definitions-in-headers)

namespace rexlog
{
    // factory methods
    template <typename Factory>
    rsl::shared_ptr<Logger> stdout_logger_mt(const rex::DebugString& loggerName)
    {
        return Factory::template create<sinks::stdout_sink_mt>(rex::DebugString(loggerName));
    }

    template <typename Factory>
    rsl::shared_ptr<Logger> stdout_logger_st(const rex::DebugString& loggerName)
    {
        return Factory::template create<sinks::stdout_sink_st>(rex::DebugString(loggerName));
    }

    template <typename Factory>
    rsl::shared_ptr<Logger> stderr_logger_mt(const rex::DebugString& loggerName)
    {
        return Factory::template create<sinks::stderr_sink_mt>(rex::DebugString(loggerName));
    }

    template <typename Factory>
    rsl::shared_ptr<Logger> stderr_logger_st(const rex::DebugString& loggerName)
    {
        return Factory::template create<sinks::stderr_sink_st>(rex::DebugString(loggerName));
    }
} // namespace rexlog

// NOLINTEND(misc-definitions-in-headers)

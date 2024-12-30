#include "rex_engine/timing/execution_logger.h"

#include "rex_engine/diagnostics/logging/log_macros.h"

namespace rex
{
  ExecutionLogger::ExecutionLogger(LogCategory category, rsl::string_view msg)
      : m_msg(msg)
      , m_log_category(category)
      , m_interval()
  {
    REX_INFO(m_log_category, rsl::format("{} - started", m_msg));
  }

  ExecutionLogger::~ExecutionLogger()
  {
    REX_INFO(m_log_category, rsl::format("{} - finished ({} ms)", m_msg, m_interval.milliseconds()));
  }
} // namespace rex
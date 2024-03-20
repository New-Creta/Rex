#pragma once

#include "rex_std/string_view.h"

#include "rex_engine/diagnostics/logging/log_category.h"
#include "rex_engine/timing/interval.h"

namespace rex
{
  class ExecutionLogger
  {
  public:
    ExecutionLogger(LogCategory category, rsl::string_view msg);
    ~ExecutionLogger();

  private:
    rsl::string_view m_msg;
    LogCategory m_log_category;
    Interval m_interval;
  };
}
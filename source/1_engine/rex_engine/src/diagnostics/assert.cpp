#include "rex_engine/diagnostics/assert.h"

#include "rex_engine/diagnostics/stacktrace.h"
#include "rex_engine/diagnostics/logging/log_macros.h"

namespace rex
{
  DECLARE_LOG_CATEGORY_EXTERN(LogAssert, LogVerbosity::Log);
  DEFINE_LOG_CATEGORY(LogAssert);

  void rex_assert(const rsl::fmt_stack_string& msg)
  {
    thread_local static bool is_processing_assert = false;
    if (!is_processing_assert)
    {
      is_processing_assert = true;
      REX_ERROR(LogAssert, "Assert Raised: {}", msg);
      ResolvedCallstack callstack(current_callstack());

      for (count_t i = 0; i < callstack.size(); ++i)
      {
        REX_ERROR(LogAssert, "{}", callstack[i]);
      }

      DEBUG_BREAK();
      //return true;
    }
    else
    {
      // if this is hit, an assert occurred while processing another one.
      // to avoid circular dependency, we break here if there's a debugger attached
      DEBUG_BREAK();
    }   
  }
}
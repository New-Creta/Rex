#include "rex_engine/diagnostics/verify.h"

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/diagnostics/win/win_stacktrace.h"
#include "tinyfiledialogs.h"

namespace rex
{
  DEFINE_LOG_CATEGORY(LogVerify, LogVerbosity::Error);

  //-------------------------------------------------------------------------
  void rex_verify(const rsl::fmt_stack_string& msg)
  {
    thread_local static bool is_processing_assert = false;
    if(!is_processing_assert)
    {
      is_processing_assert = true;

      REX_ERROR(LogVerify, "Verification Raised: {}", msg);

      REX_ERROR(LogVerify, "----------------");

      const ResolvedCallstack callstack(current_callstack());

      for(count_t i = 0; i < callstack.size(); ++i)
      {
        REX_ERROR(LogVerify, "{}", callstack[i]);
      }

      tinyfd_messageBox("Verification Failed", "The application has encountered an invalid state.", "ok", "error", 0);
    }
    else
    {
      // if this is hit, an verify occurred while processing another one.
      // to avoid circular dependency, we break here if there's a debugger attached
      DEBUG_BREAK();
    }
  }
} // namespace rex
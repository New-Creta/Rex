#include "rex_engine/diagnostics/debug.h"

#include "rex_engine/diagnostics/log.h"

namespace rex
{
  void log_sanitization()
  {
#ifdef REX_ASAN
    REX_INFO(LogEngine, "Address Sanitizer Enabled");
#else
    REX_INFO(LogEngine, "Address Sanitizer Disabled");
#endif
#ifdef REX_UBSAN
    REX_INFO(LogEngine, "Undefined Behavior Sanitizer Enabled");
#else
    REX_INFO(LogEngine, "Undefined Behavior Sanitizer Disabled");
#endif
  }

  void abort(rsl::string_view msg)
  {
    if (is_debugger_attached())
    {
      REX_DEBUG_BREAK();
    }

    output_debug_string("ABORTING APP!!");
    output_debug_string(msg);
    rsl::abort();
  }
} // namespace rex
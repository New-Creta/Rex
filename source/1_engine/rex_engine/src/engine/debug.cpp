#include "rex_engine/diagnostics/debug.h"

#include "rex_engine/diagnostics/log.h"

namespace rex
{
  void log_sanitization()
  {
#ifdef REX_ASAN
    REX_LOG(LogEngine, "Address Sanitizer Enabled");
#else
    REX_LOG(LogEngine, "Address Sanitizer Disabled");
#endif
#ifdef REX_UBSAN
    REX_LOG(LogEngine, "Undefined Behavior Enabled");
#else
    REX_LOG(LogEngine, "Undefined Behavior Disabled");
#endif
  }
}
#pragma once

namespace rex
{
  // Meant to be implement by platform specific code
  bool is_debugger_attached();
}

#ifdef REX_PLATFORM_WINDOWS
#include "rex_engine/diagnostics/win/win_debug.h"
#endif
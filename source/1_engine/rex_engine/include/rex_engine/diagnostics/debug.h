#pragma once

#include "rex_std/bonus/types.h"

namespace rex
{
  // Meant to be implement by platform specific code
  bool is_debugger_attached();

  void rex::exit_process(int32 exitCode);

  void rex::exit_confirm(int32 exitCode);
}

#ifdef REX_PLATFORM_WINDOWS
#include "rex_engine/diagnostics/win/win_debug.h"
#endif
#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/string_view.h"

namespace rex
{
  // Meant to be implement by platform specific code
  bool is_debugger_attached();

  void exit_process(int32 exitCode);

  void exit_confirm(int32 exitCode);

  void output_debug_string(rsl::string_view str);
} // namespace rex

#ifdef REX_PLATFORM_WINDOWS
  #include "rex_engine/platform/win/diagnostics/win_debug.h"
#endif
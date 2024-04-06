#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/chrono.h"
#include "rex_std/string_view.h"

namespace rex
{
  // Meant to be implement by platform specific code
  bool is_debugger_attached();

  // Exit this process immediately without going through shutdown
  void exit_process(int32 exitCode);

  // Exit this process immediately, but spawn a confirm dialog first
  void exit_confirm(int32 exitCode);

  // Output a string to the output debug window.
  void output_debug_string(rsl::string_view str);

  // Blocking function, waits for the debugger to get attached
  // Returns true if the debugger gets attached, false if not
  bool wait_for_debugger(rsl::chrono::minutes minutesToWait = rsl::chrono::minutes(10));

  // Automatically attach a debugger to this process
  void attach_debugger();

  // Log if any sanitization is enabled by the compiler
  void log_sanitization();

} // namespace rex

#ifdef REX_PLATFORM_WINDOWS
  #include "rex_engine/platform/win/diagnostics/win_debug.h"
#endif
#include "rex_engine/diagnostics/win/win_debug.h"

#include "rex_engine/diagnostics/win/win_message_box.h"

#define NOMINMAX
#include <Windows.h>
#include <debugapi.h>

bool rex::win::is_debugger_attached()
{
  return IsDebuggerPresent() > 0;
}
void rex::win::exit_process(int32 exitCode)
{
  TerminateProcess(GetCurrentProcess(), exitCode);
}
void rex::win::exit_confirm(int32 exitCode)
{
  if(yes_no_message_box("Are you sure you want to abort the process?"))
  {
    exit_process(exitCode);
  }
}
#include "rex_engine/platform/win/diagnostics/win_debug.h"

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/platform/win/diagnostics/win_message_box.h"
#include "rex_engine/diagnostics/log.h"

#define NOMINMAX
#include <Windows.h>
#include <comdef.h>
#include <debugapi.h>

bool rex::is_debugger_attached()
{
  return IsDebuggerPresent() > 0;
}
void rex::exit_process(int32 exitCode)
{
  TerminateProcess(GetCurrentProcess(), exitCode);
}
void rex::exit_confirm(int32 exitCode)
{
  if(win::yes_no_message_box("Are you sure you want to abort the process?"))
  {
    exit_process(exitCode);
  }
}

rsl::medium_stack_string rex::win::report_win_error(HResult hr, [[maybe_unused]] rsl::string_view winFunc, [[maybe_unused]] const rsl::string_view file, [[maybe_unused]] const rsl::string_view function, [[maybe_unused]] card32 lineNr)
{
  const _com_error err(hr);
  rsl::medium_stack_string error_message(err.ErrorMessage());
  REX_ERROR(LogEngine, "--- Windows Error ---");
  REX_ERROR(LogEngine, "Win API: {}", winFunc);
  REX_ERROR(LogEngine, "File: {}", file);
  REX_ERROR(LogEngine, "Function: {}", function);
  REX_ERROR(LogEngine, "On line: {}", lineNr);
  REX_ERROR(LogEngine, "Windows error: {}", error_message);
  REX_ERROR(LogEngine, "---               ---");

  return error_message;
}
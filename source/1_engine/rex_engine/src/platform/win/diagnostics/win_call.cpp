#include "rex_engine/platform/win/diagnostics/win_call.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/platform/win/diagnostics/win_debug.h"

#define NOMINMAX
#include <Windows.h>

rex::win::WinCall::WinCall(DWord /*funcResult*/, ErrorSuccess errorSuccess, rsl::string_view winFunc, rsl::string_view file, rsl::string_view function, card32 lineNr)
    : m_error(GetLastError())
    , m_error_success(errorSuccess.get())
{
  // GetLastError() is not always cleared when a function succeeds
  // So we just clear it ourselves
  // We do this here so that the logging itself won't complain about there still be an error in the pool
  clear_win_errors();

  if(has_failed())
  {
    const HRESULT hr = HRESULT_FROM_WIN32(m_error);
    m_error_message  = report_win_error(hr, winFunc, file, function, lineNr);
  }
}

rex::win::WinCall::WinCall(ErrorSuccess errorSuccess, rsl::string_view winFunc, rsl::string_view file, rsl::string_view function, card32 lineNr)
    : WinCall(0, errorSuccess, winFunc, file, function, lineNr)
{
}

bool rex::win::WinCall::has_failed() const
{
  return m_error != ERROR_SUCCESS && m_error != m_error_success;
}
bool rex::win::WinCall::has_succeeded() const
{
  return !has_failed();
}

rsl::string_view rex::win::WinCall::error_message() const
{
  return m_error_message.to_view();
}

void rex::win::check_for_win_errors(rsl::string_view file, rsl::string_view function, card32 lineNr)
{
  REX_UNUSED_PARAM(file);
  REX_UNUSED_PARAM(function);
  REX_UNUSED_PARAM(lineNr);
#ifdef REX_SUPRESS_PRE_CHECK_WINDOWS_ERRORS
  clear_win_errors();
#else
  // To check for windows error, follow the path of GetLastError.
  // It'll try to fetch a pointer to an 32bit int and put it into RAX
  // Afterwards It'll add an offset of 0x68 bytes.
  // Put a data breakpoint on this address on the range of 4 bytes
  // When it triggers, last error is set

  const DWord err = GetLastError();
  clear_win_errors();

  if(err != ERROR_SUCCESS)
  {
    const HRESULT hr = HRESULT_FROM_WIN32(err);

    REX_WARN(LogEngine, "Still Windows errors in pool!");
    report_win_error(hr, __FUNCTION__, file, function, lineNr);
  }
#endif
}

void rex::win::clear_win_errors()
{
  SetLastError(ERROR_SUCCESS);
}
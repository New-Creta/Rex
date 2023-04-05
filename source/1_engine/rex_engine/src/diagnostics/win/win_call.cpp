#include "rex_engine/diagnostics/win/win_call.h"

#include "rex_engine/diagnostics/legacy/logging.h"

#define NOMINMAX
#include <comdef.h"
#include <windows.h"

namespace rex::win
{
  rsl::medium_stack_string report_hr_error(HRESULT hr, [[maybe_unused]] const rsl::string_view file, [[maybe_unused]] const rsl::string_view function, [[maybe_unused]] card32 lineNr)
  {
    const _com_error err(hr);
    rsl::medium_stack_string error_message(err.ErrorMessage());
    REX_ERROR("Windows Error");
    REX_ERROR("File: {}", file);
    REX_ERROR("Function: {}", function);
    REX_ERROR("On line: {}", lineNr);
    REX_ERROR("Windows error: {}", error_message);

    return error_message;
  }
} // namespace rex::win

rex::win::WinCall::WinCall(rsl::string_view file, rsl::string_view function, card32 lineNr)
    : WinCall(ERROR_SUCCESS, file, function, lineNr)
{
}

rex::win::WinCall::WinCall(DWord errorSuccess, rsl::string_view file, rsl::string_view function, card32 lineNr)
    : m_error(GetLastError())
{
  if(m_error != errorSuccess && m_error != ERROR_SUCCESS)
  {
    const HRESULT hr = HRESULT_FROM_WIN32(m_error);
    m_error_message  = report_hr_error(hr, file, function, lineNr);
  }

  // GetLastError() is not always cleared when a function succeeds
  // So we just clear it ourselves
  clear_win_errors();
}

bool rex::win::WinCall::has_failed() const
{
  return m_error != ERROR_SUCCESS;
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
#ifdef REX_SUPRESS_PRE_CHECK_WINDOWS_ERRORS
  clear_win_errors();
#else
  const DWord err = GetLastError();
  clear_win_errors();

  if(err != ERROR_SUCCESS)
  {
    REX_WARN("Still Windows errors in pool!");

    const HRESULT hr = HRESULT_FROM_WIN32(err);
    report_hr_error(hr, file, function, lineNr);
  }
#endif
}

void rex::win::clear_win_errors()
{
  SetLastError(ERROR_SUCCESS);
}
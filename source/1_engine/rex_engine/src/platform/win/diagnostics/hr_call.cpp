#include "rex_engine/platform/win/diagnostics/hr_call.h"

#include "rex_engine/platform/win/diagnostics/win_debug.h"
#include "rex_std/bonus/attributes.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

rex::win::HrCall::HrCall(HResult hr, HResult ignoreSuccess, REX_MAYBE_UNUSED rsl::string_view winFunc, REX_MAYBE_UNUSED rsl::string_view file, REX_MAYBE_UNUSED rsl::string_view function, REX_MAYBE_UNUSED card32 lineNr)
    : m_hresult(hr)
    , m_hresult_to_ignore(ignoreSuccess)
    , m_has_failed(FAILED(hr))
{
  if(has_failed())
  {
    m_error_message = report_win_error(hr, winFunc, file, function, lineNr);
  }
}

bool rex::win::HrCall::has_failed() const
{
  return m_hresult != m_hresult_to_ignore && m_has_failed;
}
bool rex::win::HrCall::has_succeeded() const
{
  return !has_failed();
}

rex::win::HResult rex::win::HrCall::result() const
{
  return m_hresult;
}

rsl::string_view rex::win::HrCall::error_message() const
{
  return m_error_message.to_view();
}

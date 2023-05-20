#include "rex_engine/diagnostics/win/hr_call.h"

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/log.h"
#include "rex_std/bonus/attributes.h"

#define NOMINMAX
#include <Windows.h>
#include <comdef.h>

rex::win::HrCall::HrCall(HResult hr, REX_MAYBE_UNUSED rsl::string_view file, REX_MAYBE_UNUSED rsl::string_view function, REX_MAYBE_UNUSED card32 lineNr)
    : m_has_failed(FAILED(hr))
{
  if(has_failed())
  {
    const _com_error err(hr);
    m_error_message = err.ErrorMessage();
    REX_ERROR(LogEngine, "WINDOWS ERROR");
    REX_ERROR(LogEngine, "File: ", file);
    REX_ERROR(LogEngine, "Function: ", function);
    REX_ERROR(LogEngine, "On line: ", lineNr);
    REX_ERROR(LogEngine, "DirectX error: ", m_error_message);
  }
}

bool rex::win::HrCall::has_failed() const
{
  return m_has_failed;
}
bool rex::win::HrCall::has_succeeded() const
{
  return m_has_failed;
}

rsl::string_view rex::win::HrCall::error_message() const
{
  return m_error_message.to_view();
}

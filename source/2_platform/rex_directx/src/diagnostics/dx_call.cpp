#include "rex_directx/diagnostics/dx_call.h"

#include "rex_directx/utility/dx_util.h"
#include "rex_engine/diagnostics/log.h"

#include <comdef.h>

namespace rex
{
  namespace gfx
  {
    namespace directx
    {
      DEFINE_LOG_CATEGORY(LogDirectX);

      //-------------------------------------------------------------------------
      rsl::medium_stack_string report_hr_error(HRESULT hr, const rsl::string_view file, const rsl::string_view function, card32 lineNr)
      {
        REX_UNUSED_PARAM(file);
        REX_UNUSED_PARAM(function);
        REX_UNUSED_PARAM(lineNr);

        const _com_error err(hr);
        rsl::medium_stack_string error_message(err.ErrorMessage());
        REX_ERROR(LogDirectX, "DirectX Error");
        REX_ERROR(LogDirectX, "File: {}", file);
        REX_ERROR(LogDirectX, "Function: {}", function);
        REX_ERROR(LogDirectX, "On line: {}", lineNr);
        REX_ERROR(LogDirectX, "DirectX error: {}", error_message);

        return error_message;
      }

      //-------------------------------------------------------------------------
      DXCall::DXCall(HResult error, rsl::string_view file, rsl::string_view function, card32 lineNr)
          : m_error(error)
      {
        if(FAILED(m_error))
        {
          m_error_message = report_hr_error(m_error, file, function, lineNr);
        }
      }

      //-------------------------------------------------------------------------
      bool DXCall::has_failed() const
      {
        return FAILED(m_error);
      }

      //-------------------------------------------------------------------------
      bool DXCall::has_succeeded() const
      {
        return !has_failed();
      }

      //-------------------------------------------------------------------------
      rsl::string_view DXCall::error_message() const
      {
        return m_error_message.to_view();
      }
    } // namespace directx
  }   // namespace gfx
} // namespace rex
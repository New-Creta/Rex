// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: hr_call.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================



#include "rex_std_extra/diagnostics/win/hr_call.h"

#include "rex_std_extra/diagnostics/logging.h"

#define NOMINMAX
#include <windows.h>
#include <comdef.h>

rsl::win::HrCall::HrCall(HResult hr, const rsl::string_view file, const rsl::string_view function, card32 line_nr)
  : m_has_failed(FAILED(hr))
{
  REX_UNUSED(file, function, line_nr);

  if (has_failed())
  {
    _com_error err(hr);
    m_error_message = err.ErrorMessage();
    REX_ERROR("WINDOWS ERROR");
    REX_ERROR("File: ", file);
    REX_ERROR("Function: ", function);
    REX_ERROR("On line: ", line_nr);
    REX_ERROR("DirectX error: ", m_error_message);
  }
}

bool rsl::win::HrCall::has_failed() const
{
  return m_has_failed;
}
bool rsl::win::HrCall::has_succeeded() const
{
  return m_has_failed;
}

rsl::string_view rsl::win::HrCall::error_message() const
{
  return m_error_message.to_view();
}
// ============================================
//
// REX GAME ENGINE
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: hr_call.cpp
// Copyright (c) Nick De Breuck 2023
//
// ============================================

#include "rex_engine/diagnostics/win/hr_call.h"

#include "rex_engine/diagnostics/logging.h"

#define NOMINMAX
#include <comdef.h>
#include <windows.h>

rex::win::HrCall::HrCall(HResult hr, REX_MAYBE_UNUSED const rsl::string_view file, REX_MAYBE_UNUSED const rsl::string_view function, REX_MAYBE_UNUSED card32 line_nr)
  : m_has_failed(FAILED(hr))
{
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
// ============================================
//
// REX GAME ENGINE
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: hr_call.h
// Copyright (c) Nick De Breuck 2023
//
// ============================================

#pragma once

#include "rex_std/bonus/string/stack_string.h"
#include "rex_std/string_view.h"

namespace rex
{
  namespace win
  {
    using HResult = long;

    class HrCall
    {
    public:
      HrCall(HResult hr, const rsl::string_view file, const rsl::string_view function, card32 line_nr);

      bool has_failed() const;
      bool has_succeeded() const;

      rsl::string_view error_message() const;

    private:
      bool m_has_failed;
      rsl::medium_stack_string m_error_message;
    };
  } // namespace win
}

#ifdef REX_ENABLE_HR_CALL
#define HR_CALL(function)    rsl::win::HrCall(function, __FILE__, __FUNCTION__, __LINE__)
#define HR_SUCCESS(function) rsl::win::HrCall(function, __FILE__, __FUNCTION__, __LINE__).has_succeeded()
#define HR_FAILED(function)  rsl::win::HrCall(function, __FILE__, __FUNCTION__, __LINE__).has_failed()

#else

#define HR_CALL(function)    function
#define HR_SUCCESS(function) function
#define HR_FAILED(function)  function
#endif
// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: win_call.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/string_view.h"
#include "rex_std/bonus/string/stack_string.h"

namespace rsl
{
  namespace win
  {
    using DWord = unsigned long;

    class WinCall
    {
    public:
      WinCall(const rsl::string_view file, const rsl::string_view function, card32 line_nr);
      WinCall(DWord error_success, const rsl::string_view file, const rsl::string_view function, card32 line_nr);

      bool has_failed() const;
      bool has_succeeded() const;

      rsl::string_view error_message() const;

    private:
      DWord m_error;
      rsl::medium_stack_string m_error_message;
    };

    void check_for_win_errors(const rsl::string_view file, const rsl::string_view function, card32 line_nr);
    void clear_win_errors();

    template <typename WinObject, typename Func>
    WinObject call_to_win32_api(Func func, DWord error_success, const rsl::string_view file, const rsl::string_view function, card32 line_nr)
    {
      check_for_win_errors(file, function, line_nr);
      WinObject obj = func();
      rsl::win::WinCall(error_success, file, function, line_nr);
      return obj;
    }
  }
}

#ifdef REX_ENABLE_WIN_CALL
#define CHECK_FOR_WIN_ERRORS()        rsl::win::check_for_win_errors(__FILE__, __FUNCTION__, __LINE__)

#define WIN_CALL_IGNORE(function, error_success)  rsl::win::call_to_win32_api<decltype(function)>([&]() {return function; }, error_success, __FILE__, __FUNCTION__, __LINE__)

#define WIN_CALL(function)    rsl::win::call_to_win32_api<decltype(function)>([&]() { return function; }, ERROR_SUCCESS, __FILE__, __FUNCTION__, __LINE__)
#define WIN_SUCCESS(function) rsl::win::WinCall(function, __FILE__, __FUNCTION__, __LINE__).has_succeeded()
#define WIN_FAILED(function) rsl::win::WinCall(function, __FILE__, __FUNCTION__, __LINE__).has_failed()
#else
#define CHECK_FOR_WIN_ERRORS()        

#define WIN_CALL_IGNORE(function, error_success)  function

// calls the function, but ignores the result, assumes it's always good
#define WIN_CALL(function)    function
#define WIN_SUCCESS(function) function || true
#define WIN_FAILED(function)  function && false

#endif
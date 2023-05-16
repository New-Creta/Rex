// ============================================
//
// REX GAME ENGINE
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: win_logging.h
// Copyright (c) Nick De Breuck 2023
//
// ============================================

#pragma once

#include "rex_std/assert.h"       // for string_view
#include "rex_std/bonus/string.h" // for medium_stack_string
#include "rex_std/bonus/types.h"  // for card32
#include "rex_std/string_view.h"

namespace rex
{
  namespace win
  {
    using DWord = unsigned long;

    class WinCall
    {
    public:
      WinCall(rsl::string_view file, rsl::string_view function, card32 lineNr);
      WinCall(DWord errorSuccess, rsl::string_view file, rsl::string_view function, card32 lineNr);

      bool has_failed() const;
      bool has_succeeded() const;

      rsl::string_view error_message() const;

    private:
      DWord m_error;
      rsl::medium_stack_string m_error_message;
    };

    void check_for_win_errors(rsl::string_view file, rsl::string_view function, card32 lineNr);
    void clear_win_errors();

    template <typename WinObject, typename Func>
    WinObject call_to_win32_api(Func func, DWord errorSuccess, rsl::string_view file, rsl::string_view function, card32 lineNr)
    {
      check_for_win_errors(file, function, lineNr);
      WinObject obj = func();
      rex::win::WinCall(errorSuccess, file, function, lineNr);
      return obj;
    }
  } // namespace win
} // namespace rex

#define REX_ENABLE_WIN_CALL
#ifdef REX_ENABLE_WIN_CALL
  #define CHECK_FOR_WIN_ERRORS() rex::win::check_for_win_errors(__FILE__, __FUNCTION__, __LINE__)

  #define WIN_CALL_IGNORE(function, errorSuccess) rsl::win::call_to_win32_api<decltype(function)>([&]() { return function; }, errorSuccess, __FILE__, __FUNCTION__, __LINE__)

  #define WIN_CALL(function)    rex::win::call_to_win32_api<decltype(function)>([&]() { return function; }, ERROR_SUCCESS, __FILE__, __FUNCTION__, __LINE__)
  #define WIN_SUCCESS(function) rex::win::WinCall(function, __FILE__, __FUNCTION__, __LINE__).has_succeeded()
  #define WIN_FAILED(function)  rex::win::WinCall(function, __FILE__, __FUNCTION__, __LINE__).has_failed()

#else

  #define CHECK_FOR_WIN_ERRORS()

  #define WIN_CALL_IGNORE(function, errorSuccess) function

  #define WIN_CALL(function)    function
  #define WIN_SUCCESS(function) function
  #define WIN_FAILED(function)  function

#endif
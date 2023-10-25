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

#include "rex_engine/diagnostics/win/win_error.h"
#include "rex_engine/diagnostics/win/win_types.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/types.h"

namespace rex
{
  namespace win
  {

    REX_CREATE_STRONG_TYPE(ErrorSuccess, DWord);

    class WinCall
    {
    public:
      WinCall(DWord funcResult, ErrorSuccess errorSuccess, rsl::string_view winFunc, rsl::string_view file, rsl::string_view function, card32 lineNr);
      WinCall(ErrorSuccess errorSuccess, rsl::string_view winFunc, rsl::string_view file, rsl::string_view function, card32 lineNr);

      bool has_failed() const;
      bool has_succeeded() const;

      rsl::string_view error_message() const;

    private:
      DWord m_error;
      DWord m_error_success;
      rsl::medium_stack_string m_error_message;
    };

    void check_for_win_errors(rsl::string_view file, rsl::string_view function, card32 lineNr);
    void clear_win_errors();

    template <typename WinObject, typename Func>
    WinObject call_to_win32_api(Func func, DWord errorSuccess, rsl::string_view winFunc, rsl::string_view file, rsl::string_view function, card32 lineNr)
    {
      check_for_win_errors(file, function, lineNr);
      WinObject obj = func();
      rex::win::WinCall(ErrorSuccess(errorSuccess), winFunc, file, function, lineNr);
      return obj;
    }
  } // namespace win
} // namespace rex

#ifdef REX_ENABLE_WIN_CALL
  #define CHECK_FOR_WIN_ERRORS() rex::win::check_for_win_errors(__FILE__, __FUNCTION__, __LINE__)

// Wrapper around a Win API call.
// It'll verify internally if the call was successful.
// Used for Win API calls that do NOT return HRESULT
  #define WIN_CALL(function)    rex::win::call_to_win32_api<decltype(function)>([&]() { return function; }, static_cast<rex::win::DWord>(rex::win::Error::Success), #function, __FILE__, __FUNCTION__, __LINE__)
  #define WIN_SUCCESS(function) rex::win::WinCall(function, rex::win::ErrorSuccess(static_cast<rex::win::DWord>(rex::win::Error::Success)), #function, __FILE__, __FUNCTION__, __LINE__).has_succeeded()
  #define WIN_FAILED(function)  rex::win::WinCall(function, rex::win::ErrorSuccess(static_cast<rex::win::DWord>(rex::win::Error::Success)), #function, __FILE__, __FUNCTION__, __LINE__).has_failed()

  // Same as the above but the user is able to provide an error value to be ignored.
  // This is usefull as some Win API functions can succeed while still returning an error code
  #define WIN_CALL_IGNORE(function, errorSuccess)    rex::win::call_to_win32_api<decltype(function)>([&]() { return function; }, errorSuccess, #function, __FILE__, __FUNCTION__, __LINE__)
  #define WIN_SUCCESS_IGNORE(function, errorSuccess) rex::win::WinCall(function, rex::win::ErrorSuccess(errorSuccess), #function, __FILE__, __FUNCTION__, __LINE__).has_succeeded()
  #define WIN_FAILED_IGNORE(function, errorSuccess)  rex::win::WinCall(function, rex::win::ErrorSuccess(errorSuccess), #function, __FILE__, __FUNCTION__, __LINE__).has_failed()

#else

  #define CHECK_FOR_WIN_ERRORS()

  #define WIN_CALL(function)    function
  #define WIN_SUCCESS(function) function
  #define WIN_FAILED(function)  function

  #define WIN_CALL_IGNORE(function, errorSuccess)    function
  #define WIN_SUCCESS_IGNORE(function, errorSuccess) function, true
  #define WIN_FAILED_IGNORE(function, errorSuccess)  function, false

#endif
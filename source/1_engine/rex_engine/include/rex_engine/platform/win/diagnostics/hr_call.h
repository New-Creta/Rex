#pragma once

#include "rex_engine/platform/win/win_types.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/types.h"

namespace rex
{
  namespace win
  {

    class HrCall
    {
    public:
      HrCall(HResult hr, HResult ignoreSuccess, rsl::string_view winFunc, rsl::string_view file, rsl::string_view function, card32 lineNr);

      bool has_failed() const;
      bool has_succeeded() const;
      HResult result() const;

      rsl::string_view error_message() const;

    private:
      rsl::medium_stack_string m_error_message;
      HResult m_hresult;
      HResult m_hresult_to_ignore;
      bool m_has_failed;
    };
  } // namespace win
} // namespace rex

#ifdef REX_ENABLE_HR_CALL
  #define HR_CALL(function)    rex::win::HrCall(function, rex::win::HResult{}, rsl::string_view(#function), __FILE__, __FUNCTION__, __LINE__).result()
  #define HR_SUCCESS(function) rex::win::HrCall(function, rex::win::HResult{}, rsl::string_view(#function), __FILE__, __FUNCTION__, __LINE__).has_succeeded()
  #define HR_FAILED(function)  rex::win::HrCall(function, rex::win::HResult{}, rsl::string_view(#function), __FILE__, __FUNCTION__, __LINE__).has_failed()

  #define HR_CALL_IGNORE(function, errorSuccess)    rex::win::HrCall(function, errorSuccess, rsl::string_view(#function), __FILE__, __FUNCTION__, __LINE__).result()
  #define HR_SUCCESS_IGNORE(function, errorSuccess) rex::win::HrCall(function, errorSuccess, rsl::string_view(#function), __FILE__, __FUNCTION__, __LINE__).has_succeeded()
  #define HR_FAILED_IGNORE(function, errorSuccess)  rex::win::HrCall(function, errorSuccess, rsl::string_view(#function), __FILE__, __FUNCTION__, __LINE__).has_failed()


#else

  #define HR_CALL(function)    function
  #define HR_SUCCESS(function) function, true
  #define HR_FAILED(function)  function, false

  #define HR_CALL_IGNORE(function, errorSuccess) function
  #define HR_SUCCESS_IGNORE(function, errorSuccess) function
  #define HR_FAILED_IGNORE(function, errorSuccess) function
#endif

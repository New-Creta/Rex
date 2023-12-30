#pragma once

#include "rex_std/bonus/string.h"
#include "rex_std/bonus/types.h"

namespace rex
{
  namespace renderer
  {
    namespace directx
    {
      using HResult = long;

      class DXCall
      {
      public:
        DXCall(HResult error, HResult hrSuccess, rsl::string_view file, rsl::string_view function, card32 lineNr);

        bool has_failed() const;
        bool has_succeeded() const;

        rsl::string_view error_message() const;

      private:
        HResult m_error;
        rsl::medium_stack_string m_error_message;
      };

      namespace internal
      {
        //-------------------------------------------------------------------------
        template <typename DXObject, typename Func>
        DXObject call_to_dx_api(Func func, DXObject hrSuccess, rsl::string_view file, rsl::string_view function, card32 lineNr)
        {
          HRESULT hr = func();
          rex::renderer::directx::DXCall(hr, hrSuccess, file, function, lineNr);
          return hr;
        }

        //-------------------------------------------------------------------------
        template <typename DXObject, typename Func>
        bool call_to_dx_api_has_succeeded(Func func, DXObject hrSuccess, rsl::string_view file, rsl::string_view function, card32 lineNr)
        {
          DXObject hr = func();
          return rex::renderer::directx::DXCall(hr, hrSuccess, file, function, lineNr).has_succeeded();
        }

        //-------------------------------------------------------------------------
        template <typename DXObject, typename Func>
        bool call_to_dx_api_has_failed(Func func, DXObject hrSuccess, rsl::string_view file, rsl::string_view function, card32 lineNr)
        {
          DXObject hr = func();
          return rex::renderer::directx::DXCall(hr, hrSuccess, file, function, lineNr).has_failed();
        }
      } // namespace internal
    } // namespace directx
  } // namespace win
} // namespace rex

#define REX_ENABLE_DX_CALL
#ifdef REX_ENABLE_DX_CALL
  // This is just a wrapper around a dx call, it will log if something happened and return the result of the function call
  #define DX_CALL(function) rex::renderer::directx::internal::call_to_dx_api<decltype(function)>([&]() { return function; }, S_OK, __FILE__, __FUNCTION__, __LINE__)

  // These two macros do exactly the same thing as DX_CALL the only difference is that it will check if it was successfull or not.
  // if (DX_FAILED(...))
  // {
  //     // Do some additional logic here
  // }
  #define DX_SUCCESS(function) rex::renderer::directx::internal::call_to_dx_api_has_succeeded<decltype(function)>([&]() { return function; }, S_OK, __FILE__, __FUNCTION__, __LINE__)
  #define DX_FAILED(function)  rex::renderer::directx::internal::call_to_dx_api_has_failed<decltype(function)>([&]() { return function; }, S_OK, __FILE__, __FUNCTION__, __LINE__)

#else

  #define CHECK_FOR_DX_ERRORS()

  #define DX_CALL_IGNORE(function, hrSuccess) function

  #define DX_CALL(function)    function
  #define DX_SUCCESS(function) function
  #define DX_FAILED(function)  function

#endif
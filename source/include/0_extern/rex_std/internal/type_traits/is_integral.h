// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_integral.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/remove_cv.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T>
      struct is_integral_helper : public false_type
      {
      };

      template <>
      struct is_integral_helper<int8> : public true_type
      {
      };
      template <>
      struct is_integral_helper<int16> : public true_type
      {
      };
      template <>
      struct is_integral_helper<int32> : public true_type
      {
      };
      template <>
      struct is_integral_helper<int64> : public true_type
      {
      };

      template <>
      struct is_integral_helper<uint8> : public true_type
      {
      };
      template <>
      struct is_integral_helper<uint16> : public true_type
      {
      };
      template <>
      struct is_integral_helper<uint32> : public true_type
      {
      };
      template <>
      struct is_integral_helper<uint64> : public true_type
      {
      };

      template <>
      struct is_integral_helper<bool> : public true_type
      {
      };
      template <>
      struct is_integral_helper<char8> : public true_type
      {
      };

      // other types that aren't really used by rex engine, but we still need regardless
      template <>
      struct is_integral_helper<tchar> : public true_type
      {
      };
#ifdef REX_ENABLE_WITH_CPP20
      template <>
      struct is_integral_helper<char8_t> : public true_type
      {
      };
#endif
      template <>
      struct is_integral_helper<char16_t> : public true_type
      {
      };
      template <>
      struct is_integral_helper<char32_t> : public true_type
      {
      };

      template <>
      struct is_integral_helper<long> : public true_type
      {
      };
      template <>
      struct is_integral_helper<ulong> : public true_type
      {
      };
    } // namespace internal

    template <typename T>
    struct is_integral : public internal::is_integral_helper<remove_cv_t<T>>
    {
    };

    template <typename T>
    inline constexpr bool is_integral_v = is_integral<T>::value;

  } // namespace v1
} // namespace rsl

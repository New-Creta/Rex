// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_signed.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/is_arithmetic.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T, bool = is_arithmetic_v<T>>
      struct is_signed_helper : bool_constant<static_cast<T>(-1) < static_cast<T>(0)>
      {
      };

      template <typename T>
      struct is_signed_helper<T, false> : false_type
      {
      };
    } // namespace internal

    template <typename T>
    struct is_signed : public internal::is_signed_helper<T>
    {
    };

    template <typename T>
    inline constexpr bool is_signed_v = is_signed<T>::value;

  } // namespace v1
} // namespace rsl
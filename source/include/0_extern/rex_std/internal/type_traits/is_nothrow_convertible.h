// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_nothrow_convertible.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/is_convertible.h"
#include "rex_std/internal/type_traits/is_void.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T>
      void implicitly_convert_to(T) noexcept;

      template <typename From, typename To, bool = is_convertible_v<From, To>, bool = is_void_v<To>>
      inline constexpr bool is_no_throw_convertible_helper = noexcept(implicitly_convert_to<To>(declval<From>())); // NOLINT(cppcoreguidelines-narrowing-conversions)

      template <typename From, typename To, bool IsVoidV>
      inline constexpr bool is_no_throw_convertible_helper<From, To, false, IsVoidV> = false;

      template <typename From, typename To>
      inline constexpr bool is_no_throw_convertible_helper<From, To, true, true> = true;
    } // namespace internal

    template <typename From, typename To>
    struct is_nothrow_convertible : public bool_constant<internal::is_no_throw_convertible_helper<From, To>>
    {
    };

    template <typename From, typename To>
    inline constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<From, To>::value;

  } // namespace v1
} // namespace rsl

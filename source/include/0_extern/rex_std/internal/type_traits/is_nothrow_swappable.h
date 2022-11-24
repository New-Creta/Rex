// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_nothrow_swappable.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

/// [13/Oct/2022] RSL Comment: Remove this file, see unit tests for reason why

#include "rex_std/bonus/type_traits/is_swappable_utils.h"
#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/is_swappable.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T>
      struct is_nothrow_swappable_helper_no_except_wrapper
      {
        const static bool s_value = noexcept(swap(declval<T&>(), declval<T&>()));
      };

      template <typename T, bool>
      struct is_nothrow_swappable_helper : public bool_constant<is_nothrow_swappable_helper_no_except_wrapper<T>::value>
      {
      };

      template <typename T>
      struct is_nothrow_swappable_helper<T, false> : public false_type
      {
      };
    } // namespace internal

    template <typename T>
    struct is_nothrow_swappable : internal::is_nothrow_swappable_helper<T, is_swappable_v<T>>
    {
    };

    template <typename T>
    inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;

  } // namespace v1
} // namespace rsl

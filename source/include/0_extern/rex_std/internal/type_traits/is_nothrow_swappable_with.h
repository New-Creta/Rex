// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_nothrow_swappable_with.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

/// [13/Oct/2022] RSL Comment: Remove this file, see unit tests for reason why

#include "rex_std/bonus/type_traits/is_swappable_utils.h"
#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/is_void.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T, typename U, bool OneTypeIsVoid = (is_void_v<T> || is_void_v<U>)>
      struct is_nothrow_swappable_with_helper
      {
        static constexpr bool value = noexcept(swap(declval<T>(), declval<U>()))&& noexcept(swap(declval<U>(), declval<T>()));
      };

      template <typename T, typename U>
      struct is_nothrow_swappable_with_helper<T, U, true>
      {
        static constexpr bool value = false;
      };
    } // namespace internal

    template <typename T, typename U>
    struct is_nothrow_swappable_with : public bool_constant<internal::is_nothrow_swappable_with_helper<T, U>::value>
    {
    };

    template <typename T, typename U>
    inline constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T, U>::value;

  } // namespace v1
} // namespace rsl

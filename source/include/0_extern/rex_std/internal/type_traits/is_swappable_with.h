// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_swappable_with.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

/// [13/Oct/2022] RSL Comment: Remove this file, see unit tests for reason why

#include "rex_std/bonus/type_traits/is_swappable_utils.h"
#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/is_same.h"
#include "rex_std/internal/type_traits/is_void.h"
#include "rex_std/internal/utility/declval.h"
#include "rex_std/internal/utility/swap.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T, typename U, bool OneTypeIsvoid = (is_void_v<T> || is_void_v<U>)>
      struct is_swappable_with_helper
      {
        static constexpr bool value = !is_same_v<decltype(swap(declval<T>(), declval<U>())), unused> && !is_same_v<decltype(swap(declval<U>(), declval<T>())), unused>;
      };

      template <typename T, typename U>
      struct is_swappable_with_helper<T, U, true>
      {
        static constexpr bool value = false;
      };
    } // namespace internal

    template <typename T, typename U>
    struct is_swappable_with : public bool_constant<internal::is_swappable_with_helper<T, U>::value>
    {
    };

    template <typename T, typename U>
    inline constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;

  } // namespace v1
} // namespace rsl

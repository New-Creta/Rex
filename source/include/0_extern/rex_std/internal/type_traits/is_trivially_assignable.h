// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_trivially_assignable.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T, typename U>
    struct is_trivially_assignable : bool_constant<__is_trivially_assignable(T, U)>
    {
    };

    template <typename T, typename U>
    inline constexpr bool is_trivially_assignable_v = is_trivially_assignable<T, U>::value;

  } // namespace v1
} // namespace rsl

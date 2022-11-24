// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_trivially_constructible.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T, typename... Args>
    struct is_trivially_constructible : bool_constant<__is_trivially_constructible(T, Args...)>
    {
    };

    template <typename T, typename... Args>
    inline constexpr bool is_trivially_constructible_v = is_trivially_constructible<T, Args...>::value;

  } // namespace v1
} // namespace rsl

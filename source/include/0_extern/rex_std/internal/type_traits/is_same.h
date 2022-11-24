// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_same.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename, typename>
    inline constexpr bool is_same_v = false;
    template <typename T>
    inline constexpr bool is_same_v<T, T> = true;

    template <typename T1, typename T2>
    struct is_same : public bool_constant<is_same_v<T1, T2>>
    {
    };

  } // namespace v1
} // namespace rsl

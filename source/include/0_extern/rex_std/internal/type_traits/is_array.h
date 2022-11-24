// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_array.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename>
    inline constexpr bool is_array_v = false; // determine whether type argument is an array

    template <typename T, size_t N>
    inline constexpr bool is_array_v<T[N]> = true; // NOLINT(modernize-avoid-c-arrays)

    template <typename T>
    inline constexpr bool is_array_v<T[]> = true; // NOLINT(modernize-avoid-c-arrays)

    template <class T>
    struct is_array : public bool_constant<is_array_v<T>>
    {
    };

  } // namespace v1
} // namespace rsl

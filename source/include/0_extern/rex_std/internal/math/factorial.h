// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: factorial.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/is_integral.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    constexpr T factorial(T x)
    {
      static_assert(rsl::is_integral_v<T>, "T must be of an integer type");
      return x == 0 ? 1 : x * factorial(x - 1);
    }

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: pow.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/is_arithmetic.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    constexpr T pow(const T base, const card32 exp) // NOLINT(misc-no-recursion)
    {
      static_assert(rsl::is_arithmetic_v<T>, "T must be of an arithmetic type");

      return exp == 0 ? static_cast<T>(1) : base * pow(base, exp - 1);
    }

  } // namespace v1
} // namespace rsl

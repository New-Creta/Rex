// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: clamp_max.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/algorithm/min.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    constexpr T clamp_max(const T& x, const T& max)
    {
      return (rsl::min)(x, max);
    }

  } // namespace v1
} // namespace rsl

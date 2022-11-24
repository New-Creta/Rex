// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: clamp.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/algorithm/max.h"
#include "rex_std/internal/algorithm/min.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    constexpr T clamp(const T& x, const T& min, const T& max)
    {
      return (rsl::min)((rsl::max)(x, min), max);
    }

  } // namespace v1
} // namespace rsl

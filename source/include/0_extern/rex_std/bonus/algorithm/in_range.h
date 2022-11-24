// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: in_range.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/utility/cmp.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    bool in_range(T value, T min, T max)
    {
      return rsl::cmp_greater_equal(value, min) && rsl::cmp_less_equal(value, max);
    }

  } // namespace v1
} // namespace rsl

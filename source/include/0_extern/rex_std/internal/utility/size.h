// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: size.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T, count_t N>
    constexpr count_t size(const T (&/*arr*/)[N]) // NOLINT
    {
      return N;
    }

  } // namespace v1
} // namespace rsl

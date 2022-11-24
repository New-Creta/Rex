// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: element_literal
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {

    constexpr int32 operator"" _elem(unsigned long long int count)
    {
      return static_cast<int32>(count);
    }

  } // namespace v1
} // namespace rsl

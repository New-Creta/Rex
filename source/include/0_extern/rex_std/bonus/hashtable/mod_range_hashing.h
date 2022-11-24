// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: mod_range_hashing.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {

    struct mod_range_hashing
    {
      uint32 operator()(count_t r, uint32 n) const
      {
        return r % n;
      }
    };

  } // namespace v1
} // namespace rsl
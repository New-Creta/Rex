// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: iter_swap.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/utility/swap.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename Iterator>
    void iter_swap(Iterator lhs, Iterator rhs)
    {
      rsl::swap(*lhs, *rhs);
    }

  } // namespace v1
} // namespace rsl

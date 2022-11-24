// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: rotate_copy.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/algorithm/copy.h"

namespace rsl
{
  inline namespace v1
  {

    template <class InputIterator, class OuputIterator>
    constexpr OuputIterator rotate_copy(InputIterator first, InputIterator n_first, InputIterator last, OuputIterator d_first)
    {
      d_first = rsl::copy(n_first, last, d_first);
      return rsl::copy(first, n_first, d_first);
    }

  } // namespace v1
} // namespace rsl

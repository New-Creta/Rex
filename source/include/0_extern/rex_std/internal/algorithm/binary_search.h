// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: binary_search.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/algorithm/lower_bound.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename ForwardIterator, typename T>
    bool binary_search(ForwardIterator first, ForwardIterator last, const T& value)
    {
      // To do: This can be made slightly faster by not using lower_bound.
      ForwardIterator i(lower_bound<ForwardIterator, T>(first, last, value));
      return ((i != last) && !(value < *i)); // Note that we always express value comparisons in terms of < or ==.
    }

    template <typename ForwardIterator, typename T, typename Compare>
    bool binary_search(ForwardIterator first, ForwardIterator last, const T& value, Compare compare)
    {
      // To do: This can be made slightly faster by not using lower_bound.
      ForwardIterator i(lower_bound<ForwardIterator, T, Compare>(first, last, value, compare));
      return ((i != last) && !compare(value, *i));
    }

  } // namespace v1
} // namespace rsl

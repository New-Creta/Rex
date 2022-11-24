// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: minmax.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct min_max_result
    {
      const T& min;
      const T& max;
    };

    template <typename T>
    min_max_result<T> minmax(const T& lhs, const T& rhs)
    {
      return lhs < rhs ? min_max_result<T> {lhs, rhs} : min_max_result<T> {rhs, lhs};
    }
    template <typename T, typename Compare>
    min_max_result<T> minmax(const T& lhs, const T& rhs, Compare compare)
    {
      return compare(lhs, rhs) ? min_max_result<T> {lhs, rhs} : min_max_result<T> {rhs, lhs};
    }

  } // namespace v1
} // namespace rsl

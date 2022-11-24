// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: count.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename InputIterator, typename T>
    typename InputIterator::difference_type count(InputIterator first, InputIterator last, const T& value)
    {
      typename InputIterator::difference_type result = 0;

      for(; first != last; ++first)
      {
        if(*first == value)
          ++result;
      }
      return result;
    }

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: remove_copy.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename InputIterator, typename OutputIterator, typename T>
    OutputIterator remove_copy(InputIterator first, InputIterator last, OutputIterator d_first, const T& value)
    {
      for(; first != last; ++first)
      {
        if(!(*first == value))
        {
          *d_first++ = *first;
        }
      }
      return d_first;
    }

  } // namespace v1
} // namespace rsl

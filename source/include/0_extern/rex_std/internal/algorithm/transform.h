// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: transform.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename InputIterator, typename OutputIterator, typename Func>
    void transform(InputIterator first, InputIterator last, OutputIterator dst_first, Func func)
    {
      auto it = first;
      while(it != last)
      {
        *dst_first = func(*it);
        ++it;
        ++dst_first;
      }
    }

  } // namespace v1
} // namespace rsl

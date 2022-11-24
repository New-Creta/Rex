// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: for_each_n.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename InputIterator, typename Size, typename Func>
    InputIterator for_each_n(InputIterator first, Size n, Func func)
    {
      auto it = first;
      for(Size i = 0; i < n; ++i, ++it)
      {
        func(*it);
      }
      return it;
    }

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: generate_n.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename InputIterator, typename Size, typename Generator>
    void generate_n(InputIterator first, InputIterator last, Size n, Generator g)
    {
      auto it = first;
      for(Size i = 0; i < n; ++i, ++it)
      {
        *it = g();
      }
    }

  } // namespace v1
} // namespace rsl

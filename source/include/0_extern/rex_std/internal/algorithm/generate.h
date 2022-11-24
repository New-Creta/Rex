// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: generate.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename InputIterator, typename Generator>
    void generate(InputIterator first, InputIterator last, Generator g)
    {
      auto it = first;
      while(it != last)
      {
        *it = g();
        ++it;
      }
    }

  } // namespace v1
} // namespace rsl

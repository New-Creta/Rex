// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: all_of.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename InputIterator, typename Predicate>
    bool all_of(InputIterator first, InputIterator last, Predicate p)
    {
      for(; first != last; ++first)
      {
        if(!p(*first))
          return false;
      }
      return true;
    }

  } // namespace v1
} // namespace rsl

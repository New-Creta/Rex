// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: find.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename InputIterator, typename T>
    InputIterator find(InputIterator first, InputIterator last, const T& value)
    {
      auto it = first;
      while(it != last)
      {
        if(*it == value)
        {
          return it;
        }
        ++it;
      }

      return it;
    }

  } // namespace v1
} // namespace rsl

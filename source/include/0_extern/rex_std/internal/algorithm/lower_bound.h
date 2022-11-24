// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: lower_bound.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename InputIterator, typename T>
    InputIterator lower_bound(InputIterator first, InputIterator last, const T& value)
    {
      auto it = first; // NOLINT
      while(it != last)
      {
        if(!(*it < value))
        {
          return it;
        }
        ++it;
      }

      return it;
    }

    template <typename InputIterator, typename T, typename Compare>
    InputIterator lower_bound(InputIterator first, InputIterator last, const T& value, Compare comp)
    {
      auto it = first;
      while(it != last)
      {
        if(comp(*it, value) == false)
        {
          return it;
        }
        ++it;
      }

      return it;
    }

  } // namespace v1
} // namespace rsl

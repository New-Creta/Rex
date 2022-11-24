// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: upper_bound.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/iterator/advance.h"
#include "rex_std/internal/iterator/distance.h"
#include "rex_std/internal/iterator/iterator_traits.h"

namespace rsl
{
  inline namespace v1
  {

    template <class ForwardIterator, class T>
    ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value)
    {
      ForwardIterator it;
      using diff_type = typename iterator_traits<ForwardIterator>::difference_type;
      diff_type count = 0;
      diff_type step  = 0;
      count           = rsl::distance(first, last);

      while(count > 0)
      {
        it   = first;
        step = count / 2;
        rsl::advance(it, step);
        if(!(value < *it))
        {
          first = ++it;
          count -= step + 1;
        }
        else
        {
          count = step;
        }
      }
      return first;
    }

    template <class ForwardIterator, class T, class Compare>
    ForwardIterator upper_bound(ForwardIterator first, ForwardIterator last, const T& value, Compare comp)
    {
      ForwardIterator it;
      typename iterator_traits<ForwardIterator>::difference_type count, step;
      count = rsl::distance(first, last);

      while(count > 0)
      {
        it   = first;
        step = count / 2;
        rsl::advance(it, step);
        if(!comp(value, *it))
        {
          first = ++it;
          count -= step + 1;
        }
        else
        {
          count = step;
        }
      }
      return first;
    }

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: equal_range.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/algorithm/lower_bound.h"
#include "rex_std/internal/algorithm/upper_bound.h"
#include "rex_std/internal/iterator/advance.h"
#include "rex_std/internal/iterator/distance.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename Iterator>
    struct EqualrangeResult
    {
      Iterator first_it;
      Iterator second_it;
    };

    template <typename Iterator, typename T>
    EqualrangeResult<Iterator> equal_range(Iterator first, Iterator last, const T& value)
    {
      using ResultType      = EqualrangeResult<Iterator>;
      using difference_type = typename Iterator::difference_type;

      difference_type d = rsl::distance(first, last);

      while(d > 0)
      {
        Iterator i(first);
        difference_type d2 = d >> 1; // We use '>>1' here instead of '/2' because MSVC++ for some reason generates significantly worse code for '/2'. Go figure.

        rsl::advance(i, d2);

        if(*i < value)
        {
          first = ++i;
          d -= d2 + 1;
        }
        else if(value < *i)
        {
          d    = d2;
          last = i;
        }
        else
        {
          Iterator j(i);

          return ResultType(rsl::lower_bound(first, i, value), rsl::upper_bound(++j, last, value));
        }
      }
      return ResultType(first, first);
    }

    template <typename Iterator, typename T, typename Compare>
    EqualrangeResult<Iterator> equal_range(Iterator first, Iterator last, const T& value, Compare compare)
    {
      using ResultType      = EqualrangeResult<Iterator>;
      using difference_type = typename Iterator::difference_type;

      difference_type d = rsl::distance(first, last);

      while(d > 0)
      {
        Iterator i(first);
        difference_type d2 = d >> 1; // We use '>>1' here instead of '/2' because MSVC++ for some reason generates significantly worse code for '/2'. Go figure.

        rsl::advance(i, d2);

        if(compare(*i, value))
        {
          first = ++i;
          d -= d2 + 1;
        }
        else if(compare(value, *i))
        {
          d    = d2;
          last = i;
        }
        else
        {
          Iterator j(i);

          return ResultType(rsl::lower_bound(first, i, value, compare), rsl::upper_bound(++j, last, value, compare));
        }
      }
      return ResultType(first, first);
    }

  } // namespace v1
} // namespace rsl

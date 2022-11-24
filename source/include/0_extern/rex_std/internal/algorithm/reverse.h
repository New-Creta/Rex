// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: reverse.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/algorithm/iter_swap.h"
#include "rex_std/internal/iterator/iterator_tags.h"
#include "rex_std/internal/type_traits/is_base_of.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename InputIterator>
    void reverse(InputIterator first, InputIterator last)
    {
      using iter_cat = typename InputIterator::iterator_category;

      if constexpr(is_base_of_v<random_access_iterator_tag, iter_cat>)
      {
        if(first == last)
          return;

        for(--last; first < last; (void)++first, --last)
        {
          iter_swap(first, last);
        }
      }
      else
      {
        while((first != last) && (first != --last))
        {
          iter_swap(first++, last);
        }
      }
    }

  } // namespace v1
} // namespace rsl

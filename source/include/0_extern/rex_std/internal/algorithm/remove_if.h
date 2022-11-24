// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: remove_if.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/algorithm/find_if.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename InputIterator, typename Predicate>
    InputIterator remove_if(InputIterator first, InputIterator last, Predicate predicate)
    {
      first = rsl::find_if(first, last, predicate);
      if(first != last)
      {
        for(InputIterator it = first; ++it != last;)
        {
          if(!predicate(*it))
          {
            *first++ = rsl::move(*it);
          }
        }
      }
      return first;
    }

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: remove.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/algorithm/find.h"
#include "rex_std/internal/utility/move.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename InputIterator, typename T>
    InputIterator remove(InputIterator first, InputIterator last, const T& value)
    {
      first = rsl::find(first, last, value);

      if(first != last)
      {
        for(InputIterator it = first; ++it != last;)
        {
          if(*it != value)
          {
            *first++ = rsl::move(*it);
          }
        }
      }

      return first;
    }

  } // namespace v1
} // namespace rsl

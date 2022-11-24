// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: reverse_copy.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/iterator/iterator_tags.h"
#include "rex_std/internal/type_traits/is_base_of.h"

namespace rsl
{
  inline namespace v1
  {

    template <class InputIterator, class OutputIterator>
    constexpr OutputIterator reverse_copy(InputIterator first, InputIterator last, OutputIterator d_first)
    {
      while(first != last)
      {
        *(d_first++) = *(--last);
      }

      return d_first;
    }

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: none_of.h
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
    bool none_of(const InputIterator first, const InputIterator last, Predicate predicate)
    {
      return find_if(first, last, predicate) == last;
    }

  } // namespace v1
} // namespace rsl

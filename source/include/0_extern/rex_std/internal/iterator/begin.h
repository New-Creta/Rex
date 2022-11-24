// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: begin.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename C>
    auto begin(C& c) -> decltype(c.begin())
    {
      return c.begin();
    }
    template <typename C>
    auto begin(const C& c) -> decltype(c.begin())
    {
      return c.begin();
    }
    template <typename T, card32 N>
    T* begin(T (&array)[N]) // NOLINT(modernize-avoid-c-arrays)
    {
      return &array[0];
    }
    template <typename C>
    auto cbegin(const C& c) -> decltype(c.cbegin())
    {
      return c.cbegin();
    }

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: type_and.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <bool B1, bool B2, bool B3 = true, bool B4 = true, bool B5 = true>
    struct type_and;

    template <bool B1, bool B2, bool B3, bool B4, bool B5>
    struct type_and
    {
      static constexpr bool value = false;
    };

    template <>
    struct type_and<true, true, true, true, true>
    {
      static constexpr bool value = true;
    };

  } // namespace v1
} // namespace rsl

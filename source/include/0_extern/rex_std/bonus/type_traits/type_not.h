// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: type_not.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <bool B>
    struct type_not
    {
      static constexpr bool value = true;
    };

    template <>
    struct type_not<true>
    {
      static constexpr bool value = false;
    };

  } // namespace v1
} // namespace rsl

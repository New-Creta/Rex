// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: type_equal.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <int b1, int b2>
    struct type_equal
    {
      static const bool value = (b1 == b2);
    };

  } // namespace v1
} // namespace rsl

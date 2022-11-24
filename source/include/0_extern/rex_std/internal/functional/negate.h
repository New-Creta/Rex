// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: negate.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename T = void>
    struct negate
    {
      constexpr T operator()(const T& arg) const
      {
        return -arg;
      }
    };

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: div_result.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct DivResult
    {
      T quot;
      T rem;
    };

    template <typename T>
    constexpr DivResult<T> div(T x, T y)
    {
      DivResult<T> res {};
      res.quot = x / y;
      res.rem  = x % y;
      return res;
    }

  } // namespace v1
} // namespace rsl

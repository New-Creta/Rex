// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: use_self.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct use_self
    {
      using result_type = T;

      const T& operator()(const T& x) const
      {
        return x;
      }
    };

  } // namespace v1
} // namespace rsl
// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_constant_evaluated.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    inline constexpr bool is_constant_evaluated()
    {
      return __builtin_is_constant_evaluated();
    }

  } // namespace v1
} // namespace rsl

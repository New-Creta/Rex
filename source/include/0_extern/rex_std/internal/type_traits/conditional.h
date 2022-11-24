// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: conditional.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <bool Test, typename T1, typename T2>
    struct conditional
    {
      using type = T1;
    };

    template <typename T1, typename T2>
    struct conditional<false, T1, T2>
    {
      using type = T2;
    };

    template <bool Test, typename T1, typename T2>
    using conditional_t = typename conditional<Test, T1, T2>::type;

  } // namespace v1
} // namespace rsl

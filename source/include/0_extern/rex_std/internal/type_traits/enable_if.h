// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: enable_if.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <bool Test, typename T = void>
    struct enable_if
    {
    };

    template <typename T>
    struct enable_if<true, T>
    {
      using type = T;
    };

    template <bool Test, typename T = void>
    using enable_if_t = typename enable_if<Test, T>::type;

  } // namespace v1
} // namespace rsl

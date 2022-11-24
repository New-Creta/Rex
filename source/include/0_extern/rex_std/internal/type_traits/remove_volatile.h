// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: remove_volatile.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct remove_volatile
    {
      using type = T;
    };

    template <typename T>
    struct remove_volatile<volatile T>
    {
      using type = T;
    };

    template <typename T>
    using remove_volatile_t = typename remove_volatile<T>::type;

  } // namespace v1
} // namespace rsl

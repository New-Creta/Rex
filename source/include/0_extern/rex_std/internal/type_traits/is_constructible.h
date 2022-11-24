// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_constructible.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename T, typename... Args>
    struct is_constructible : public bool_constant<__is_constructible(T, Args...)>
    {
    };

    template <typename T, typename... Args>
    inline constexpr bool is_constructible_v = is_constructible<T, Args...>::value;

  } // namespace v1
} // namespace rsl

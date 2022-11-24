// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_trivially_copyable.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct is_trivially_copyable
    {
      static inline constexpr bool value = __is_trivially_copyable(T);
    };

    template <typename T>
    inline constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;

  } // namespace v1
} // namespace rsl

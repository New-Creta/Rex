// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_class.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct is_class : public bool_constant<__is_class(T)>
    {
    };

    template <typename T>
    inline constexpr bool is_class_v = is_class<T>::value;

  } // namespace v1
} // namespace rsl

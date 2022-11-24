// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_enum.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct is_enum : public bool_constant<__is_enum(T)>
    {
    };

    template <typename T>
    inline constexpr bool is_enum_v = is_enum<T>::value;

  } // namespace v1
} // namespace rsl

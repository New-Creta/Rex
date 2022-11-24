// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_standard_layout.h
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
    struct is_standard_layout : public bool_constant<__is_standard_layout(T)>
    {
    };

    template <typename T>
    inline constexpr bool is_standard_layout_v = is_standard_layout<T>::value;

  } // namespace v1
} // namespace rsl

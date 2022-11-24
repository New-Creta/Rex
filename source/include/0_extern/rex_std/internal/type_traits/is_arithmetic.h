// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_arithmetic.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/is_floating_point.h"
#include "rex_std/internal/type_traits/is_integral.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct is_arithmetic : public bool_constant<is_integral_v<T> || is_floating_point_v<T>>
    {
    };

    template <typename T>
    inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

  } // namespace v1
} // namespace rsl

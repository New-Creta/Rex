// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_nothrow_default_constructible.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/is_nothrow_constructible.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct is_nothrow_default_constructible : bool_constant<is_nothrow_constructible_v<T>>
    {
    };

    template <typename T>
    inline constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<T>::value;

  } // namespace v1
} // namespace rsl

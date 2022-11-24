// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_trivial.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/is_trivially_copyable.h"
#include "rex_std/internal/type_traits/is_trivially_default_constructible.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct is_trivial : public bool_constant<__is_trivially_constructible(T) && __is_trivially_copyable(T)>
    {
    };

    template <typename T>
    inline constexpr bool is_trivial_v = is_trivial<T>::value;

  } // namespace v1
} // namespace rsl

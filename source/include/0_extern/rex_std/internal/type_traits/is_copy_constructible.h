// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_copy_constructible.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/add_lvalue_reference.h"
#include "rex_std/internal/type_traits/is_constructible.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct is_copy_constructible : bool_constant<__is_constructible(T, add_lvalue_reference_t<const T>)>
    {
    };

    template <typename T>
    inline constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;

  } // namespace v1
} // namespace rsl

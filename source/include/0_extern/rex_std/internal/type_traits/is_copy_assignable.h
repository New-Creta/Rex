// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_copy_assignable.h
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
    struct is_copy_assignable : bool_constant<__is_assignable(add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>)>
    {
    };

    template <typename T>
    inline constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_scalar.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/type_traits/is_null_pointer.h"
#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/is_arithmetic.h"
#include "rex_std/internal/type_traits/is_enum.h"
#include "rex_std/internal/type_traits/is_member_pointer.h"
#include "rex_std/internal/type_traits/is_pointer.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct is_scalar : public bool_constant<is_arithmetic_v<T> || is_enum_v<T> || is_pointer_v<T> || is_member_pointer_v<T> || is_null_pointer_v<T>>
    {
    };

    template <typename T>
    struct is_scalar<T*> : public true_type
    {
    };
    template <typename T>
    struct is_scalar<T* const> : public true_type
    {
    };
    template <typename T>
    struct is_scalar<T* volatile> : public true_type
    {
    };
    template <typename T>
    struct is_scalar<T* const volatile> : public true_type
    {
    };

    template <typename T>
    inline constexpr bool is_scalar_v = is_scalar<T>::value;

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_rvalue_reference.h
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
    struct is_rvalue_reference : public false_type
    {
    };
    template <typename T>
    struct is_rvalue_reference<T&&> : public true_type
    {
    };

    template <typename T>
    inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

  } // namespace v1
} // namespace rsl

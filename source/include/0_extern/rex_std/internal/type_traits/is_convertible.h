// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_convertible.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename From, typename To>
    struct is_convertible : public bool_constant<__is_convertible_to(From, To)>
    {
    };

    template <typename From, typename To>
    inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

  } // namespace v1
} // namespace rsl

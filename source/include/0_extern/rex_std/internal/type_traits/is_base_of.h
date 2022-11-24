// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_base_of.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename Base, typename Derived>
    struct is_base_of : public bool_constant<__is_base_of(Base, Derived)>
    {
    };

    template <typename Base, typename Derived>
    inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

  } // namespace v1
} // namespace rsl

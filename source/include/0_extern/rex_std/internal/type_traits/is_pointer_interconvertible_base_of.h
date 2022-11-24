// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_pointer_interconvertible_base_of.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#ifdef REX_ENABLE_WITH_CPP20

  #include "rex_std/internal/type_traits/integral_constant.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename Base, typename Derived>
    struct is_pointer_inter_convertible_base_of : public bool_constant<__is_pointer_interconvertible_base_of(Base, Derived)>
    {
    };

    template <typename Base, typename Derived>
    inline constexpr bool is_pointer_inter_convertible_base_of_v = is_pointer_inter_convertible_base_of<Base, Derived>::value;

  } // namespace v1
} // namespace rsl

#endif
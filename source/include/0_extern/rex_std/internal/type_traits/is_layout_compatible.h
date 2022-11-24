// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_layout_compatible.h
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

    template <typename T, typename U>
    struct is_layout_compatible : public bool_constant<__is_layout_compatible(T, U)>
    {
    };

    template <typename T, typename U>
    inline constexpr bool is_layout_compatible_v = is_layout_compatible<T, U>::value;

  } // namespace v1
} // namespace rsl

#endif
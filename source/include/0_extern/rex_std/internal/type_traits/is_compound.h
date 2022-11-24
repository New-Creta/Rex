// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_compound.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/is_fundamental.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct is_compound : public bool_constant<!is_fundamental_v<T>>
    {
    };

    template <typename T>
    inline constexpr bool is_compound_v = is_compound<T>::value;

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: negation.h
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
    struct negation : bool_constant<!static_cast<bool>(T::value)>
    {
    };

    template <typename T>
    inline constexpr bool negation_v = negation<T>::value;

  } // namespace v1
} // namespace rsl

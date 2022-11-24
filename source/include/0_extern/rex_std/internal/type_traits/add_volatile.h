// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: add_volatile.h
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
    struct add_volatile
    {
      using type = volatile T;
    };

    template <typename T>
    using add_volatile_t = typename add_volatile<T>::type;

  } // namespace v1
} // namespace rsl

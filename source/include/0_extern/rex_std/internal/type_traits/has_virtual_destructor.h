// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: has_virtual_destructor.h
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
    struct has_virtual_destructor : public bool_constant<__has_virtual_destructor(T)>
    {
    };

    template <typename T>
    inline constexpr bool has_virtual_destructor_v = has_virtual_destructor<T>::value;

  } // namespace v1
} // namespace rsl

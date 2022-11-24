// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_void.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/is_same.h"
#include "rex_std/internal/type_traits/remove_cv.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct is_void : public bool_constant<is_same_v<remove_cv_t<T>, void>>
    {
    };

    template <typename T>
    inline constexpr bool is_void_v = is_void<T>::value;

  } // namespace v1
} // namespace rsl

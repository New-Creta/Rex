// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_const.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"

namespace rsl
{
  inline namespace v1
  {

    // determine whether type argument is const qualified

    namespace internal
    {
      template <typename T>
      struct is_const_value : public false_type
      {
      };
      template <typename T>
      struct is_const_value<const T*> : public true_type
      {
      };
      template <typename T>
      struct is_const_value<const volatile T*> : public true_type
      {
      };
    } // namespace internal

    template <typename T>
    struct is_const : public internal::is_const_value<T*>
    {
    };
    template <typename T>
    struct is_const<T&> : public false_type
    {
    };

    template <typename T>
    inline constexpr bool is_const_v = is_const<T>::value;

  } // namespace v1
} // namespace rsl

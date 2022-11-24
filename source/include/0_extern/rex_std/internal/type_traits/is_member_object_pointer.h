// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_member_object_pointer.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/is_function.h"
#include "rex_std/internal/type_traits/remove_cv.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename>
      struct is_member_object_pointer
      {
        static inline constexpr bool value = false;
      };

      template <typename T1, typename T2>
      struct is_member_object_pointer<T1 T2::*>
      {
        static inline constexpr bool value = !is_function_v<T1>;
        using class_type                   = T2;
      };
    } // namespace internal

    template <typename T>
    inline constexpr bool is_member_object_pointer_v = internal::is_member_object_pointer<remove_cv_t<T>>::value;

    template <typename T>
    struct is_member_object_pointer : bool_constant<is_member_object_pointer_v<T>>
    {
    };

  } // namespace v1
} // namespace rsl

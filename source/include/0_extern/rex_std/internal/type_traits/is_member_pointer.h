// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_member_pointer.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/remove_cv.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename>
      struct is_member_pointer_helper : public false_type
      {
      };

      template <typename T, typename U>
      struct is_member_pointer_helper<T U::*> : public true_type
      {
      };
    } // namespace internal

    template <typename T>
    struct is_member_pointer : public internal::is_member_pointer_helper<remove_cv_t<T>>::type
    {
    };

    template <typename T>
    inline constexpr bool is_member_pointer_v = is_member_pointer<T>::value;

  } // namespace v1
} // namespace rsl

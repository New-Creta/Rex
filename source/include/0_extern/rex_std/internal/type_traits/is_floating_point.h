// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_floating_point.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/remove_cv.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T>
      struct is_floating_point_helper : public false_type
      {
      };

      template <>
      struct is_floating_point_helper<float32> : public true_type
      {
      };
      template <>
      struct is_floating_point_helper<float64> : public true_type
      {
      };

      // other types that aren't really used by rex engine, but we still need regardless
      template <>
      struct is_floating_point_helper<lfloat64> : public true_type
      {
      };
    } // namespace internal

    template <typename T>
    struct is_floating_point : public internal::is_floating_point_helper<typename rsl::remove_cv_t<T>>
    {
    };

    template <typename T>
    inline constexpr bool is_floating_point_v = is_floating_point<T>::value;

  } // namespace v1
} // namespace rsl

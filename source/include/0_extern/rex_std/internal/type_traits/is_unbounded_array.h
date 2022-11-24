// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_unbounded_array.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/internal/type_traits/integral_constant.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct is_unbounded_array : public false_type
    {
    };

    template <typename T>
    struct is_unbounded_array<T[]> : public true_type // NOLINT(modernize-avoid-c-arrays)
    {
    };

    template <typename T>
    inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: alignment_of.h
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
    struct alignment_of : integral_constant<size_t, alignof(T)>
    {
    };

    template <typename T>
    inline constexpr size_t alignment_of_v = alignof(T);

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: declval.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/utility/always_false.h"
#include "rex_std/internal/type_traits/add_rvalue_reference.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    add_rvalue_reference_t<T> declval() noexcept
    {
      static_assert(internal::always_false<T>, "declval cannot be called in an evaluated expression");
    }

  } // namespace v1
} // namespace rsl

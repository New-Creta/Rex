// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: move_if_noexcept.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/conditional.h"
#include "rex_std/internal/type_traits/is_copy_constructible.h"
#include "rex_std/internal/type_traits/is_nothrow_move_constructible.h"
#include "rex_std/internal/utility/move.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    constexpr conditional_t<!is_nothrow_move_constructible_v<T> && is_copy_constructible_v<T>, const T&, T&&> move_if_noexcept(T& arg)
    {
      return move(arg);
    }

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: addressof.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/attributes.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    REX_NO_DISCARD constexpr T* addressof(T& val)
    {
      return __builtin_addressof(val);
    }
    template <typename T>
    REX_NO_DISCARD constexpr const T* addressof(const T& val) // Not in the standard
    {
      return __builtin_addressof(val);
    }

    template <typename T>
    REX_NO_DISCARD const T* addressof(const T&&) = delete;

  } // namespace v1
} // namespace rsl

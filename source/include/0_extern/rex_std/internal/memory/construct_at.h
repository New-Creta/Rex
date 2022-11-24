// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: construct_at.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/memory/construct_at
//
// Create a T object initialized with arguments args at given address p.
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/internal/utility/forward.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T, typename... Args>
    constexpr T* construct_at(T* p, Args&&... args)
    {
      return ::new(const_cast<void*>(static_cast<const volatile void*>(p))) T(rsl::forward<Args>(args)...); // NOLINT(cppcoreguidelines-pro-type-const-cast)
    }

  } // namespace v1
} // namespace rsl

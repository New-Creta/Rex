// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: identity.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/attributes.h"
#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {

    struct identity
    {
    public:
      using is_transparent = int32;

      template <typename T>
      REX_NO_DISCARD constexpr T&& operator()(T&& arg) const
      {
        return rsl::forward<T>(arg);
      }
    };

  } // namespace v1
} // namespace rsl

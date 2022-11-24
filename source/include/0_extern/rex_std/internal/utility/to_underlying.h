// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: to_underlying.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/internal/type_traits/underlying_type.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename Enum>
    constexpr underlying_type_t<Enum> to_underlying(Enum e)
    {
      return static_cast<underlying_type_t<Enum>>(e);
    }

  } // namespace v1
} // namespace rsl

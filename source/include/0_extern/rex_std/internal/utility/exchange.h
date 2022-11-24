// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: exchange.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/utility/forward.h"
#include "rex_std/internal/utility/move.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T, typename Other = T>
    constexpr T exchange(T& val, Other&& newVal)
    {
      T old_val = rsl::move(val);
      val       = rsl::forward<Other>(newVal);
      return old_val;
    }

  } // namespace v1
} // namespace rsl

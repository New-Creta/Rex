// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: clamp.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/algorithm/max.h"
#include "rex_std/internal/algorithm/min.h"
#include "rex_std/internal/type_traits/type_identity.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    constexpr T clamp(const T& x, const rsl::type_identity_t<T>& min, const rsl::type_identity_t<T>& max)
    {
      return rsl::min(rsl::max(x, min), max);
    }

    template <typename T>
    constexpr T clamp_min(const T& x, const rsl::type_identity_t<T>& min)
    {
      return max(x, min);
    }
    template <typename T>
    constexpr T clamp_max(const T& x, const rsl::type_identity_t<T>& max)
    {
      return min(x, max);
    }

  } // namespace v1
} // namespace rsl

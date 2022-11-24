// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_corresponding_member.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"

#ifdef REX_ENABLE_WITH_CPP20

namespace rsl
{
  inline namespace v1
  {

    template <typename S1, typename S2, typename M1, typename M2>
    constexpr bool is_corresponding_member(M1 S1::*pm1, M2 S2::*pm2) noexcept
    {
      return __is_corresponding_member(S1, S2, pm1, pm2);
    }

  } // namespace v1
} // namespace rsl

#endif
// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_invocable_r.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/type_traits/is_invocable_utils.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename R, typename F, typename... Args>
    struct is_invocable_r : public internal::is_invocable_r_impl<R, F, void, Args...>
    {
    };

    template <typename R, typename F, typename... Args>
    inline constexpr bool is_invocable_r_v = is_invocable_r<R, F, Args...>::value;

  } // namespace v1
} // namespace rsl

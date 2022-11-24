// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_nothrow_invocable_r.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/type_traits/is_invocable_utils.h"
#include "rex_std/internal/type_traits/is_nothrow_invocable.h"

namespace rsl
{
  inline namespace v1
  {
    namespace internal
    {
      template <typename R, typename F, typename... Args>
      struct is_nothrow_invocable_r_impl<R, F, void_t<typename invoke_result<F, Args...>::type>, Args...>
      {
        static constexpr bool value = rsl::is_convertible<typename rsl::invoke_result<F, Args...>::type, R>::value && is_nothrow_invocable<F, Args...>::value;
      };
    } // namespace internal

    template <typename R, typename F, typename... Args>
    struct is_nothrow_invocable_r : public internal::is_nothrow_invocable_r_impl<R, F, void, Args...>
    {
    };

    template <typename R, typename F, typename... Args>
    inline constexpr bool is_nothrow_invocable_r_v = is_nothrow_invocable_r<R, F, Args...>::value;
  } // namespace v1
} // namespace rsl

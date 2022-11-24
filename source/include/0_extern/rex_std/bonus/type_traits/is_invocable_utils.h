// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_invocable_utils.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/functional/invoke.h"
#include "rex_std/internal/type_traits/invoke_result.h"
#include "rex_std/internal/utility/declval.h"

namespace rsl
{
  inline namespace v1
  {
    namespace internal
    {
      template <typename F, typename = void, typename... Args>
      struct is_invocable_impl : public rsl::false_type
      {
      };

      template <typename F, typename... Args>
      struct is_invocable_impl<F, void_t<typename invoke_result<F, Args...>::type>, Args...> : public rsl::true_type
      {
      };

      template <typename R, typename F, typename = void, typename... Args>
      struct is_invocable_r_impl : public rsl::false_type
      {
      };

      template <typename R, typename F, typename... Args>
      struct is_invocable_r_impl<R, F, void_t<typename invoke_result<F, Args...>::type>, Args...> : public is_convertible<typename invoke_result<F, Args...>::type, R>
      {
      };

      template <typename F, typename = void, typename... Args>
      struct is_nothrow_invocable_impl : public rsl::false_type
      {
      };

      template <typename F, typename... Args>
      struct is_nothrow_invocable_impl<F, void_t<typename invoke_result<F, Args...>::type>, Args...> : public rsl::bool_constant<noexcept(invoke(rsl::declval<F>(), rsl::declval<Args>()...))>
      {
      };

      template <typename R, typename F, typename = void, typename... Args>
      struct is_nothrow_invocable_r_impl : public rsl::false_type
      {
      };
    } // namespace internal

  } // namespace v1
} // namespace rsl

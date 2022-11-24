// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: invoke_result.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/enable_if.h"
#include "rex_std/internal/type_traits/is_base_of.h"
#include "rex_std/internal/type_traits/is_convertible.h"
#include "rex_std/internal/type_traits/void.h"
#include "rex_std/internal/utility/declval.h"
#include "rex_std/internal/utility/forward.h"

namespace rsl
{
  inline namespace v1
  {
    namespace internal
    {

      template <typename R, typename C, typename T, typename... Args>
      constexpr auto invoke_impl(R C::*func, T&& obj, Args&&... args) -> typename enable_if<is_base_of<C, decay_t<T>>::value, decltype((rsl::forward<T>(obj).*func)(rsl::forward<Args>(args)...))>::type // NOLINT(fuchsia-trailing-return)
      {
        return (rsl::forward<T>(obj).*func)(rsl::forward<Args>(args)...);
      }

      template <typename F, typename... Args>
      constexpr auto invoke_impl(F&& func, Args&&... args) -> decltype(rsl::forward<F>(func)(rsl::forward<Args>(args)...)) // NOLINT(fuchsia-trailing-return)
      {
        return rsl::forward<F>(func)(rsl::forward<Args>(args)...);
      }

      template <typename R, typename C, typename T, typename... Args>
      constexpr auto invoke_impl(R C::*func, T&& obj, Args&&... args) -> decltype(((*rsl::forward<T>(obj)).*func)(rsl::forward<Args>(args)...)) // NOLINT(fuchsia-trailing-return)
      {
        return ((*rsl::forward<T>(obj)).*func)(rsl::forward<Args>(args)...);
      }

      template <typename M, typename C, typename T>
      constexpr auto invoke_impl(M C::*member, T&& obj) -> typename enable_if<is_base_of<C, decay_t<T>>::value, decltype(rsl::forward<T>(obj).*member)>::type // NOLINT(fuchsia-trailing-return)
      {
        return rsl::forward<T>(obj).*member;
      }

      template <typename M, typename C, typename T>
      constexpr auto invoke_impl(M C::*member, T&& obj) -> decltype((*rsl::forward<T>(obj)).*member)
      {
        return (*rsl::forward<T>(obj)).*member;
      }

      template <typename F, typename... Args>
      constexpr decltype(auto) invoke(F&& func, Args&&... args)
      {
        return invoke_impl(rsl::forward<F>(func), rsl::forward<Args>(args)...);
      }

      template <typename F, typename = void, typename... Args>
      struct invoke_result_impl
      {
      };

      template <typename F, typename... Args>
      struct invoke_result_impl<F, void_t<decltype(invoke_impl(rsl::declval<F>(), rsl::declval<Args>()...))>, Args...>
      {
        using type = decltype(invoke_impl(rsl::declval<F>(), rsl::declval<Args>()...));
      };
    } // namespace internal

    template <typename F, typename... Args>
    struct invoke_result : public internal::invoke_result_impl<F, void, Args...>
    {
    };

    template <typename F, typename... Args>
    using invoke_result_t = typename invoke_result<F, Args...>::type;

  } // namespace v1
} // namespace rsl

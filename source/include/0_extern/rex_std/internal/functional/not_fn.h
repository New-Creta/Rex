// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: not_fn.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename F>
      struct not_fn_t
      {
        F f; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
        template <typename... Args>
        constexpr auto operator()(Args&&... args) & -> decltype(!rsl::invoke(f, rsl::forward<Args>(args)...))
        {
          return !rsl::invoke(f, rsl::forward<Args>(args)...);
        }

        template <typename... Args>
        constexpr auto operator()(Args&&... args) const& -> decltype(!rsl::invoke(f, rsl::forward<Args>(args)...))
        {
          return !rsl::invoke(f, rsl::forward<Args>(args)...);
        }

        template <typename... Args>
        constexpr auto operator()(Args&&... args) && -> decltype(!rsl::invoke(rsl::move(f), rsl::forward<Args>(args)...))
        {
          return !rsl::invoke(rsl::move(f), rsl::forward<Args>(args)...);
        }

        template <typename... Args>
        constexpr auto operator()(Args&&... args) const&& -> decltype(!rsl::invoke(rsl::move(f), rsl::forward<Args>(args)...))
        {
          return !rsl::invoke(rsl::move(f), rsl::forward<Args>(args)...);
        }
      };
    } // namespace internal

    // Creates a forwarding call wrapper that returns the negation of the callable object it holds.
    template <typename F>
    constexpr internal::not_fn_t<F> not_fn(F&& f)
    {
      return {rsl::forward<F>(f)};
    }

  } // namespace v1
} // namespace rsl

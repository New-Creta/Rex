// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: extent.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/integral_constant.h"

namespace rsl
{
  inline namespace v1
  {
    namespace internal
    {
      template <typename T, unsigned N>
      struct extent_help : public rsl::integral_constant<size_t, 0>
      {
      };

      template <typename T, unsigned I>
      struct extent_help<T[I], 0> : public rsl::integral_constant<size_t, I> // NOLINT(modernize-avoid-c-arrays)
      {
      }; // NOLINT(modernize-avoid-c-arrays)

      template <typename T, unsigned N, unsigned I>
      struct extent_help<T[I], N> : public extent_help<T, N - 1> // NOLINT(modernize-avoid-c-arrays)
      {
      }; // NOLINT(modernize-avoid-c-arrays)

      template <typename T, unsigned N>
      struct extent_help<T[], N> : public extent_help<T, N - 1> // NOLINT(modernize-avoid-c-arrays)
      {
      }; // NOLINT(modernize-avoid-c-arrays)
    }    // namespace internal

    template <typename T, unsigned N = 0>                   // extent uses unsigned instead of size_t.
    struct extent : public rsl::internal::extent_help<T, N> // NOLINT(modernize-avoid-c-arrays)
    {
    };

    template <typename T, unsigned N = 0>
    inline constexpr auto extent_v = extent<T, N>::value;

  } // namespace v1
} // namespace rsl

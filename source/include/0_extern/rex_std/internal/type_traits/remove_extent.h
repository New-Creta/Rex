// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: remove_extent.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct remove_extent
    {
      using type = T;
    };

    template <typename T, size_t I>
    struct remove_extent<T[I]> // NOLINT(modernize-avoid-c-arrays)
    {
      using type = T;
    };

    template <typename T>
    struct remove_extent<T[]> // NOLINT(modernize-avoid-c-arrays)
    {
      using type = T;
    };

    template <typename T>
    using remove_extent_t = typename remove_extent<T>::type;

  } // namespace v1
} // namespace rsl

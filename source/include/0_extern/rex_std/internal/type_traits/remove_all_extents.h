// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: remove_all_extents.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct remove_all_extents
    {
      using type = T;
    };

    template <typename T, size_t N>
    struct remove_all_extents<T[N]> // NOLINT(modernize-avoid-c-arrays)
    {
      using type = typename remove_all_extents<T>::type;
    };

    template <typename T>
    struct remove_all_extents<T[]> // NOLINT(modernize-avoid-c-arrays)
    {
      using type = typename remove_all_extents<T>::type;
    };

    template <typename T>
    using remove_all_extents_t = typename remove_all_extents<T>::type;

  } // namespace v1
} // namespace rsl

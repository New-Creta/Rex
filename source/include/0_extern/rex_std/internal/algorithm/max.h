// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: max.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    constexpr const T&(max)(const T& lhs, const T& rhs)
    {
      return lhs > rhs ? lhs : rhs;
    }
    template <typename T, typename Compare>
    constexpr const T&(max)(const T& lhs, const T& rhs, Compare comp)
    {
      return comp(lhs, rhs) ? rhs : lhs;
    }

  } // namespace v1
} // namespace rsl

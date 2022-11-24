// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: treat_as_floating_point.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/chrono/treat_as_floating_point
//
// Helps determine if a duration can be converted to another duration with a different tick period.
//-----------------------------------------------------------------------------

#pragma once

namespace rsl
{
  inline namespace v1
  {
    namespace chrono
    {
      template <typename Rep>
      struct treat_as_floating_point : public is_floating_point<Rep>
      {
      };

      template <typename Rep>
      inline constexpr bool tread_at_floating_point_v = treat_as_floating_point<Rep>::value;
    } // namespace chrono
  }   // namespace v1
} // namespace rsl
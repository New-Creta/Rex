// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: duration_values.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/limits.h"

namespace rsl
{
  inline namespace v1
  {

    namespace chrono
    {
      template <typename Rep>
      struct duration_values
      {
      public:
        static constexpr Rep zero()
        {
          return Rep(0);
        }
        static constexpr Rep(max)()
        {
          return (rsl::numeric_limits<Rep>::max)();
        }
        static constexpr Rep(min)()
        {
          return (rsl::numeric_limits<Rep>::min)();
        }
      };

    } // namespace chrono

  } // namespace v1
} // namespace rsl

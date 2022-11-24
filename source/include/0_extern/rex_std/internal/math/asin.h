// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: asin.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/math/return_type.h"
#include "rex_std/bonus/types.h"
#include "rex_std/limits.h"
#include "rex_std/numbers.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    constexpr T asin_compute(const T x)
    {
      return ( // only defined on [-1,1]
          x > T(1) ? numeric_limits<T>::quiet_NaN() :
                   // indistinguishable from one or zero
              numeric_limits<T>::min() > abs(x - T(1)) ? T(rsl::half_pi_v<float64>)
          : numeric_limits<T>::min() > abs(x)          ? T(0)
                                                       :
                                              // else
              atan(x / sqrt(T(1) - x * x)));
    }

    template <typename T>
    constexpr T asin_check(const T x)
    {
      return ( // NaN check
          is_nan(x) ? numeric_limits<T>::quiet_NaN() :
                    //
              x < T(0) ? -asin_compute(-x)
                       : asin_compute(x));
    }

    /**
     * Compile-time arcsine function
     *
     * @param x a real-valued input, where \f$ x \in [-1,1] \f$.
     * @return the inverse sine function using \f[ \text{asin}(x) = \text{atan} \left( \frac{x}{\sqrt{1-x^2}} \right) \f]
     */

    template <typename T>
    constexpr return_t<T> asin(const T x)
    {
      return internal::asin_check(static_cast<return_t<T>>(x));
    }

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: acos.h
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

    namespace internal
    {
      template <typename T>
      constexpr T acos_compute(const T x)
      {
        return ( // only defined on [-1,1]
            abs(x) > T(1) ? numeric_limits<T>::quiet_NaN() :
                          // indistinguishable from one or zero
                numeric_limits<T>::min() > abs(x - T(1)) ? T(0)
            : numeric_limits<T>::min() > abs(x)          ? T(rsl::half_pi_v<float64>)
                                                         :
                                                // else
                atan(sqrt(T(1) - x * x) / x));
      }

      template <typename T>
      constexpr T acos_check(const T x)
      {
        return ( // NaN check
            is_nan(x) ? numeric_limits<T>::quiet_NaN() :
                      //
                x > T(0) ? // if
                acos_compute(x)
                         :
                         // else
                T(rsl::pi_v<float64>) - acos_compute(-x));
      }

    } // namespace internal

    /**
     * Compile-time arccosine function
     *
     * @param x a real-valued input, where \f$ x \in [-1,1] \f$.
     * @return the inverse cosine function using \f[ \text{acos}(x) = \text{atan} \left( \frac{\sqrt{1-x^2}}{x} \right) \f]
     */

    template <typename T>
    constexpr return_t<T> acos(const T x)
    {
      return internal::acos_check(static_cast<return_t<T>>(x));
    }

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: sqrt.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/math/return_type.h"
#include "rex_std/limits.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T>
      constexpr T sqrt_recur(const T x, const T xn, const int count)
      {
        return (abs(xn - x / xn) / (T(1) + xn) < numeric_limits<T>::min() ? // if
                    xn
                                                                          : count < GCEM_SQRT_MAX_ITER ? // else
                                                                                sqrt_recur(x, T(0.5) * (xn + x / xn), count + 1)
                                                                                                       : xn);
      }

      template <typename T>
      constexpr T sqrt_check(const T x, const T m_val)
      {
        return (is_nan(x) ? numeric_limits<T>::quiet_NaN() :
                          //
                    x < T(0) ? numeric_limits<T>::quiet_NaN()
                             :
                             //
                    is_posinf(x) ? x
                                 :
                                 // indistinguishable from zero or one
                    numeric_limits<T>::min() > abs(x)      ? T(0)
                : numeric_limits<T>::min() > abs(T(1) - x) ? x
                                                           :
                                                           // else
                    x > T(4) ? sqrt_check(x / T(4), T(2) * m_val)
                             : m_val * sqrt_recur(x, x / T(2), 0));
      }

    } // namespace internal

    /**
     * Compile-time square-root function
     *
     * @param x a real-valued input.
     * @return Computes \f$ \sqrt{x} \f$ using a Newton-Raphson approach.
     */

    template <typename T>
    constexpr return_t<T> sqrt(const T x)
    {
      return internal::sqrt_check(static_cast<return_t<T>>(x), return_t<T>(1));
    }

  } // namespace v1
} // namespace rsl

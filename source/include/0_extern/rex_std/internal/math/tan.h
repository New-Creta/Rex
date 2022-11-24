// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: tan.h
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
      constexpr T tan_series_exp_long(const T z)
      { // this is based on a fourth-order expansion of tan(z) using Bernoulli numbers
        return (-1 / z + (z / 3 + (pow_integral(z, 3) / 45 + (2 * pow_integral(z, 5) / 945 + pow_integral(z, 7) / 4725))));
      }

      template <typename T>
      constexpr T tan_series_exp(const T x)
      {
        return (numeric_limits<T>::min() > abs(x - T(rsl::half_pi_v<float64>)) ? // the value tan(pi/2) is somewhat of a convention;
                                                                                 // technically the function is not defined at EXACTLY pi/2,
                                                                                 // but this is floating point pi/2
                    T(1.633124e+16)
                                                                               :
                                                                               // otherwise we use an expansion around pi/2
                    tan_series_exp_long(x - T(rsl::half_pi_v<float64>)));
      }

      template <typename T>
      constexpr T tan_cf_recur(const T xx, const int depth, const int max_depth)
      {
        return (depth < max_depth ? // if
                    T(2 * depth - 1) - xx / tan_cf_recur(xx, depth + 1, max_depth)
                                  :
                                  // else
                    T(2 * depth - 1));
      }

      template <typename T>
      constexpr T tan_cf_main(const T x)
      {
        return ((x > T(1.55) && x < T(1.60)) ? tan_series_exp(x) : // deals with a singularity at tan(pi/2)
                                                                   //
                    x > T(1.4) ? x / tan_cf_recur(x * x, 1, 45)
                : x > T(1)     ? x / tan_cf_recur(x * x, 1, 35)
                               :
                           // else
                    x / tan_cf_recur(x * x, 1, 25));
      }

      template <typename T>
      constexpr T tan_begin(const T x, const int count = 0)
      {                                                          // tan(x) = tan(x + pi)
        return (x > T(rsl::pi_v<float64>) ?                      // if
                    count > 1 ? numeric_limits<T>::quiet_NaN() : // protect against undefined behavior
                        tan_begin(x - T(rsl::pi_v<float64>) * internal::floor_check(x / T(rsl::pi_v<float64>)), count + 1)
                                          :
                                          // else
                    tan_cf_main(x));
      }

      template <typename T>
      constexpr T tan_check(const T x)
      {
        return ( // NaN check
            is_nan(x) ? numeric_limits<T>::quiet_NaN() :
                      // indistinguishable from zero
                numeric_limits<T>::min() > abs(x) ? T(0)
                                                  :
                                                  // else
                x < T(0) ? -tan_begin(-x)
                         : tan_begin(x));
      }

    } // namespace internal

    /**
     * Compile-time tangent function
     *
     * @param x a real-valued input.
     * @return the tangent function using
     * \f[ \tan(x) = \dfrac{x}{1 - \dfrac{x^2}{3 - \dfrac{x^2}{5 - \ddots}}} \f]
     * To deal with a singularity at \f$ \pi / 2 \f$, the following expansion is employed:
     * \f[ \tan(x) = - \frac{1}{x-\pi/2} - \sum_{k=1}^\infty \frac{(-1)^k 2^{2k} B_{2k}}{(2k)!} (x - \pi/2)^{2k - 1} \f]
     * where \f$ B_n \f$ is the n-th Bernoulli number.
     */

    template <typename T>
    constexpr return_t<T> tan(const T x)
    {
      return internal::tan_check(static_cast<return_t<T>>(x));
    }

  } // namespace v1
} // namespace rsl

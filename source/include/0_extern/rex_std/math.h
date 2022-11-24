// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: math.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/math/abs.h"

namespace rsl
{
  inline namespace v1
  {

    using float_t  = float32;
    using double_t = float64;

#define HUGE_ENUF 1e+300

#ifndef INFINITY
  #define INFINITY ((float32)(HUGE_ENUF * HUGE_ENUF))
#endif

#ifndef HUGE_VAL
  #define HUGE_VAL ((float64)INFINITY)
#endif
#ifndef HUGE_VALF
  #define HUGE_VALF ((float32)INFINITY)
#endif
#ifndef HUGE_VALL
  #define HUGE_VALL ((lfloat64)INFINITY)
#endif

#ifndef NAN
  #define NAN (-(float32)(INFINITY * 0.0f))
#endif

#define MATH_ERRNO       1
#define MATH_ERREXCEPT   2
#define math_errhandling (MATH_ERRNO | MATH_ERREXCEPT)

#ifndef FP_INFINITE
  #define FP_INFINITE 1
#endif
#ifndef FP_NAN
  #define FP_NAN 2
#endif
#ifndef FP_NORMAL
  #define FP_NORMAL (-1)
#endif
#ifndef FP_SUBNORMAL
  #define FP_SUBNORMAL (-2)
#endif
#ifdef FP_ZERO
  #define FP_ZERO 0
#endif
    namespace internal
    {
      template <typename T>
      T fmod(T x, T div)
      {
        // if div or x is nan, return x directly
        if(div != div || x != x)
        {
          return x;
        }

        if(div == 0)
        {
          return NAN;
        }

        return x - (int)(x / div) * div;
      }

      template <typename T>
      T sqrt(T arg)
      {
        REX_ASSERT_X(arg >= (T)0, "sqrt called on negative value");

        float64 s, last;

        if(arg > 0)
        {
          if(arg > 1)
          {
            s = static_cast<float64>(arg);
          }
          else
          {
            s = 1;
          }

          do
          {
            last = s;
            s    = static_cast<float64>((arg / s + s) / 2);
          } while(s < last);

          return static_cast<T>(last);
        }

        return 0;
      }
    } // namespace internal

    float32 abs(float32 arg)
    {
      return abs(arg);
    }
    float64 abs(float64 arg)
    {
      return abs(arg);
    }
    lfloat64 abs(lfloat64 arg)
    {
      return abs(arg);
    }

    float32 fabs(float32 arg)
    {
      return abs(arg);
    }
    float32 fabsf(float32 arg)
    {
      return abs(arg);
    }
    float64 fabs(float64 arg)
    {
      return abs(arg);
    }
    lfloat64 fabs(lfloat64 arg)
    {
      return abs(arg);
    }
    lfloat64 fabsl(lfloat64 arg)
    {
      return abs(arg);
    }

    template <typename IntegralType>
    float64 fabs(IntegralType arg)
    {
      return abs(static_cast<float64>(arg));
    }

    float32 fmod(float32 x, float32 div)
    {
      return internal::fmod(x, div);
    }
    float32 fmodf(float32 x, float32 div)
    {
      return internal::fmod(x, div);
    }
    float64 fmod(float64 x, float64 div)
    {
      return internal::fmod(x, div);
    }
    lfloat64 fmod(lfloat64 x, lfloat64 div)
    {
      return internal::fmod(x, div);
    }
    lfloat64 fmodl(lfloat64 x, lfloat64 div)
    {
      return internal::fmod(x, div);
    }
    template <typename Arithmetic>
    auto fmod(Arithmetic x, Arithmetic div)
    {
      return internal::fmod(static_cast<float64>(x), static_cast<float64>(div));
    }

    float32 sqrt(float32 arg)
    {
      return internal::sqrt(arg);
    }
    float32 sqrtf(float32 arg)
    {
      return internal::sqrt(arg);
    }
    float64 sqrt(float64 arg)
    {
      return internal::sqrt(arg);
    }
    lfloat64 sqrt(lfloat64 arg)
    {
      return internal::sqrt(arg);
    }
    lfloat64 sqrtl(lfloat64 arg)
    {
      return internal::sqrt(arg);
    }
    template <typename IntegralType>
    float64 sqrt(IntegralType arg)
    {
      return internal::sqrt(arg);
    }

    float32 sin(float32 arg);
    float32 sinf(float32 arg);
    float64 sin(float64 arg);
    lfloat64 sin(lfloat64 arg);
    lfloat64 sinl(lfloat64 arg);
    template <typename IntegralType>
    float64 sin(IntegralType arg);

    float32 cos(float32 arg);
    float32 cosf(float32 arg);
    float64 cos(float64 arg);
    lfloat64 cos(lfloat64 arg);
    lfloat64 cosl(lfloat64 arg);
    template <typename IntegralType>
    float64 cos(IntegralType arg);

    float32 tan(float32 arg);
    float32 tanf(float32 arg);
    float64 tan(float64 arg);
    lfloat64 tan(lfloat64 arg);
    lfloat64 tanl(lfloat64 arg);
    template <typename IntegralType>
    float64 tan(IntegralType arg);

    float32 asin(float32 arg);
    float32 asinf(float32 arg);
    float64 asin(float64 arg);
    lfloat64 asin(lfloat64 arg);
    lfloat64 asinl(lfloat64 arg);
    template <typename IntegralType>
    float64 asin(IntegralType arg);

    float32 acos(float32 arg);
    float32 acosf(float32 arg);
    float64 acos(float64 arg);
    lfloat64 acos(lfloat64 arg);
    lfloat64 acosl(lfloat64 arg);
    template <typename IntegralType>
    float64 acos(IntegralType arg);

    float32 atan(float32 arg);
    float32 atanf(float32 arg);
    float64 atan(float64 arg);
    lfloat64 atan(lfloat64 arg);
    lfloat64 atanl(lfloat64 arg);
    template <typename IntegralType>
    float64 atan(IntegralType arg);

  } // namespace v1
} // namespace rsl

#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <cmath>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_FUNC_ALIAS(remainder);
    REX_STD_FUNC_ALIAS(remquo);
    REX_STD_FUNC_ALIAS(fma);
    REX_STD_FUNC_ALIAS(fmax);
    REX_STD_FUNC_ALIAS(fmin);
    REX_STD_FUNC_ALIAS(fdim);
    REX_STD_FUNC_ALIAS(nan);
    REX_STD_FUNC_ALIAS(lerp);
    REX_STD_FUNC_ALIAS(exp2);
    REX_STD_FUNC_ALIAS(expm1);
    REX_STD_FUNC_ALIAS(log);
    REX_STD_FUNC_ALIAS(log10);
    REX_STD_FUNC_ALIAS(log2);
    REX_STD_FUNC_ALIAS(log1p);
    REX_STD_FUNC_ALIAS(atan2);
    REX_STD_FUNC_ALIAS(sinh);
    REX_STD_FUNC_ALIAS(cosh);
    REX_STD_FUNC_ALIAS(tanh);
    REX_STD_FUNC_ALIAS(asinh);
    REX_STD_FUNC_ALIAS(acosh);
    REX_STD_FUNC_ALIAS(atanh);
    REX_STD_FUNC_ALIAS(erf);
    REX_STD_FUNC_ALIAS(erfc);
    REX_STD_FUNC_ALIAS(tgamma);
    REX_STD_FUNC_ALIAS(lgamma);
    REX_STD_FUNC_ALIAS(trunc);
    REX_STD_FUNC_ALIAS(round);
    REX_STD_FUNC_ALIAS(nearbyint);
    REX_STD_FUNC_ALIAS(rint);
    REX_STD_FUNC_ALIAS(frexp);
    REX_STD_FUNC_ALIAS(ldexp);
    REX_STD_FUNC_ALIAS(modf);
    REX_STD_FUNC_ALIAS(scalbn);
    REX_STD_FUNC_ALIAS(ilogb);
    REX_STD_FUNC_ALIAS(logb);
    REX_STD_FUNC_ALIAS(nextafter);
    REX_STD_FUNC_ALIAS(copysign);
    REX_STD_FUNC_ALIAS(fpclassify);
    REX_STD_FUNC_ALIAS(isfinite);
    REX_STD_FUNC_ALIAS(isinf);
    REX_STD_FUNC_ALIAS(isnormal);
    REX_STD_FUNC_ALIAS(isgreater);
    REX_STD_FUNC_ALIAS(isgreaterequal);
    REX_STD_FUNC_ALIAS(isless);
    REX_STD_FUNC_ALIAS(islessequal);
    REX_STD_FUNC_ALIAS(islessgreater);
    REX_STD_FUNC_ALIAS(isunordered);
    REX_STD_FUNC_ALIAS(assoc_laguerre);
    REX_STD_FUNC_ALIAS(assoc_legendre);
    REX_STD_FUNC_ALIAS(beta);
    REX_STD_FUNC_ALIAS(comp_ellint_1);
    REX_STD_FUNC_ALIAS(comp_ellint_2);
    REX_STD_FUNC_ALIAS(comp_ellint_3);
    REX_STD_FUNC_ALIAS(expint);
    REX_STD_FUNC_ALIAS(hermite);
    REX_STD_FUNC_ALIAS(legendre);
    REX_STD_FUNC_ALIAS(laguerre);
    REX_STD_FUNC_ALIAS(riemann_zeta);
    REX_STD_FUNC_ALIAS(sph_bessel);
    REX_STD_FUNC_ALIAS(sph_legendre);
    REX_STD_FUNC_ALIAS(sph_neumann);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
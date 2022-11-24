// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: ratio.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/type_traits/sign_of.h"
#include "rex_std/bonus/types.h"
#include "rex_std/internal/type_traits/integral_constant.h"

namespace rsl
{
  inline namespace v1
  {

    template <intmax Num, intmax Den>
    struct ratio;

    namespace internal
    {
      template <intmax Val>
      struct abs : integral_constant<intmax, (Val < 0 ? -Val : Val)>
      {
      };

      template <intmax X, intmax Y>
      struct gcd_x : gcd_x<Y, X % Y>::type
      {
      };

      template <intmax Val>
      struct gcd_x<Val, 0> : integral_constant<intmax, Val>
      {
      };

      template <intmax X, intmax Y>
      struct gcd : gcd_x<abs<X>::value, abs<Y>::value>::type
      {
      };

      // specialization to avoid division by 0
      template <>
      struct gcd<0, 0> : integral_constant<intmax, 1>
      {
      };

      template <typename T>
      inline constexpr bool is_ratio = false;

      template <intmax R1, intmax R2>
      inline constexpr bool is_ratio<ratio<R1, R2>> = true;

      template <intmax X, intmax Y>
      struct lcm
      {
        static constexpr intmax value = (X * (Y / gcd<X, Y>::value));
      };

      template <typename Period1, typename Period2>
      struct ratio_gcd
      {
        static_assert(is_ratio<Period1>, "Period1 is not a ratio type");
        static_assert(is_ratio<Period2>, "Period2 is not a ratio type");

        using type = ratio<gcd<Period1::num, Period2::num>::value, lcm<Period1::den, Period2::den>::value>;
      };

      template <intmax X, intmax Y>
      struct addition_over_flow
      {
        static const bool s_different_signs    = (X <= 0 && 0 <= Y) || (Y < 0 && 0 < X);
        static const bool s_inrange_of_int_max = abs<Y>::value <= INTMAX_MAX - abs<X>::value;
        static const bool s_value              = s_different_signs || s_inrange_of_int_max;
      };

      template <intmax X, intmax Y>
      struct multiply_over_flow
      {
        static const bool s_value = (abs<X>::value <= (INTMAX_MAX / abs<Y>::value));
      };

      template <intmax X, intmax Y>
      struct ct_add
      {
        static_assert(addition_over_flow<X, Y>::value, "compile-time addition overflow");
        static const intmax s_value = X + Y;
      };

      template <intmax X, intmax Y>
      struct ct_sub
      {
        static_assert(addition_over_flow<X, -Y>::value, "compile-time addition overflow");
        static const intmax s_value = X - Y;
      };

      template <intmax X, intmax Y>
      struct ct_multi
      {
        static_assert(multiply_over_flow<X, Y>::s_value, "compile-time multiply overflow");
        static const intmax s_value = X * Y;
      };

      template <class R1>
      struct ct_simplify
      {
        static const intmax s_divisor = internal::gcd<R1::num, R1::den>::value;
        static const intmax s_num     = R1::num / s_divisor;
        static const intmax s_den     = R1::den / s_divisor;

        using ratio_type = ratio<s_num, s_den>;
        using this_type  = ct_simplify<R1>;
      };
    } // namespace internal

    template <intmax Num, intmax Den = 1>
    struct ratio
    {
      static_assert(Den != 0, "zero denominator");
      static_assert(Num > -INTMAX_MAX, "numerator too negative");
      static_assert(Den > -INTMAX_MAX, "denominator too negative");

      static constexpr intmax num = sign_of_v<Num> * sign_of_v<Den> * internal::abs<Num>::value / internal::gcd<Num, Den>::value;
      static constexpr intmax den = internal::abs<Den>::value / internal::gcd<Num, Den>::value;

      using type = ratio<num, den>;
    };

    template <typename R1, typename R2>
    struct ratio_add_struct
    {
      using type = typename internal::ct_simplify<typename ratio<internal::ct_add<internal::ct_multi<R1::num, R2::den>::value, internal::ct_multi<R2::num, R1::den>::value>::value, internal::ct_multi<R1::den, R2::den>::value>::type>::ratio_type;
    };

    template <typename R1, typename R2>
    struct ratio_subtract_struct
    {
      using type = typename internal::ct_simplify<
          typename ratio<internal::ct_sub<internal::ct_sub<internal::ct_multi<R1::num, R2::den>::value, internal::ct_multi<R2::num, R1::den>::value>::value, internal::ct_multi<R1::den, R2::den>::value>::value>::type>::ratio_type;
    };

    template <typename R1, typename R2>
    struct ratio_multiply_struct
    {
      using type = typename internal::ct_simplify<typename ratio<internal::ct_multi<R1::num, R2::num>::s_value, internal::ct_multi<R1::den, R2::den>::s_value>::type>::ratio_type;
    };

    template <typename R1, typename R2>
    struct ratio_divide_struct
    {
      using type = typename internal::ct_simplify<typename ratio<internal::ct_multi<R1::num, R2::den>::s_value, internal::ct_multi<R1::den, R2::num>::s_value>::type>::ratio_type;
    };

    template <typename R1, typename R2>
    struct ratio_equal_struct
    {
      using sr1 = internal::ct_simplify<R1>;
      using sr2 = internal::ct_simplify<R2>;

      static const bool s_value = (sr1::num == sr2::num) && (sr1::den == sr2::den);
    };

    template <typename R1, typename R2>
    struct ratio_less_struct
    {
      static const bool s_value = (R1::num * R2::den) < (R2::num * R1::den);
    };

    template <typename R1, typename R2>
    using RatioAdd = typename ratio_add_struct<R1, R2>::type;
    template <typename R1, typename R2>
    using RatioSubtract = typename ratio_subtract_struct<R1, R2>::type;
    template <typename R1, typename R2>
    using RatioMultiply = typename ratio_multiply_struct<R1, R2>::type;
    template <typename R1, typename R2>
    using RatioDivide = typename ratio_divide_struct<R1, R2>::type;

    template <typename R1, typename R2>
    struct ratio_equal : public bool_constant<ratio_equal_struct<R1, R2>::value>
    {
    };
    template <typename R1, typename R2>
    struct ratio_not_equal : public bool_constant<!ratio_equal<R1, R2>::value>
    {
    };
    template <typename R1, typename R2>
    struct ratio_less : public bool_constant<ratio_less_struct<R1, R2>::value>
    {
    };
    template <typename R1, typename R2>
    struct ratio_less_equal : public bool_constant<!ratio_less<R2, R1>::value>
    {
    };
    template <typename R1, typename R2>
    struct ratio_greater : public bool_constant<ratio_less<R2, R1>::value>
    {
    };
    template <typename R1, typename R2>
    struct ratio_greater_equal : public bool_constant<!ratio_less<R1, R2>::value>
    {
    };

    // using yocto = ratio<1, 1000000000000000000000000>;   // not supported, too big for intmax
    // using zepto = ratio<1, 1000000000000000000000   >;   // not supported, too big for intmax
    using atto  = ratio<1, 1000000000000000000>;
    using femto = ratio<1, 1000000000000000>;
    using pico  = ratio<1, 1000000000000>;
    using nano  = ratio<1, 1000000000>;
    using micro = ratio<1, 1000000>;
    using milli = ratio<1, 1000>;
    using centi = ratio<1, 100>;
    using deci  = ratio<1, 10>;
    using deca  = ratio<10, 1>;
    using hecto = ratio<100, 1>;
    using kilo  = ratio<1000, 1>;
    using mega  = ratio<1000000, 1>;
    using giga  = ratio<1000000000, 1>;
    using tera  = ratio<1000000000000, 1>;
    using peta  = ratio<1000000000000000, 1>;
    using exa   = ratio<1000000000000000000, 1>;
    // using zetta = ratio<1000000000000000000000, 1   >;   // not supported, too big for intmax
    // using yotta = ratio<1000000000000000000000000, 1>;   // not supported, too big for intmax

  } // namespace v1
} // namespace rsl

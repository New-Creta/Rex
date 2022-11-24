// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: numbers.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/bonus/utility/always_false.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T>
      struct Invalid
      {
        static_assert(internal::always_false<T>, "A program that instantiates a primary template of a mathematical constant variable template is ill-formed");
      };
    } // namespace internal

    template <typename T>
    constexpr T e_v = internal::Invalid<T> {};
    template <typename T>
    constexpr T log2e_v = internal::Invalid<T> {};
    template <typename T>
    constexpr T log10e_v = internal::Invalid<T> {};
    template <typename T>
    constexpr T pi_v = internal::Invalid<T> {};
    template <typename T>
    constexpr T log2pi_v = internal::Invalid<T> {}; /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <typename T>
    constexpr T log_sqrt_2pi_v = internal::Invalid<T> {}; /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <typename T>
    constexpr T half_pi_v = internal::Invalid<T> {}; /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <typename T>
    constexpr T sqrt_pi_v = internal::Invalid<T> {}; /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <typename T>
    constexpr T sqrt_half_pi_v = internal::Invalid<T> {}; /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <typename T>
    constexpr T inv_pi_v = internal::Invalid<T> {};
    template <typename T>
    constexpr T inv_sqrtpi_v = internal::Invalid<T> {};
    template <typename T>
    constexpr T ln2_v = internal::Invalid<T> {};
    template <typename T>
    constexpr T ln10_v = internal::Invalid<T> {};
    template <typename T>
    constexpr T sqrt2_v = internal::Invalid<T> {};
    template <typename T>
    constexpr T sqrt3_v = internal::Invalid<T> {};
    template <typename T>
    constexpr T inv_sqrt3_v = internal::Invalid<T> {};
    template <typename T>
    constexpr T egamma_v = internal::Invalid<T> {};
    template <typename T>
    constexpr T phi_v = internal::Invalid<T> {};

    template <>
    constexpr float64 e_v<float64> = 2.718281828459045;
    template <>
    constexpr float64 log2e_v<float64> = 1.4426950408889634;
    template <>
    constexpr float64 log10e_v<float64> = 0.4342944819032518;
    template <>
    constexpr float64 pi_v<float64> = 3.141592653589793;
    template <>
    constexpr float64 log2pi_v<float64> = 1.8378770664093454; /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <>
    constexpr float64 log_sqrt_2pi_v<float64> = 0.9189385332046727; /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <>
    constexpr float64 half_pi_v<float64> = 1.5707963267948966; /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <>
    constexpr float64 sqrt_pi_v<float64> = 1.7724538509055160; /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <>
    constexpr float64 sqrt_half_pi_v<float64> = 1.2533141373155002; /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <>
    constexpr float64 inv_pi_v<float64> = 0.3183098861837907;
    template <>
    constexpr float64 inv_sqrtpi_v<float64> = 0.5641895835477563;
    template <>
    constexpr float64 ln2_v<float64> = 0.6931471805599453;
    template <>
    constexpr float64 ln10_v<float64> = 2.302585092994046;
    template <>
    constexpr float64 sqrt2_v<float64> = 1.4142135623730951;
    template <>
    constexpr float64 sqrt3_v<float64> = 1.7320508075688772;
    template <>
    constexpr float64 inv_sqrt3_v<float64> = 0.5773502691896257;
    template <>
    constexpr float64 egamma_v<float64> = 0.5772156649015329;
    template <>
    constexpr float64 phi_v<float64> = 1.618033988749895;

    template <>
    constexpr float32 e_v<float32> = static_cast<float32>(e_v<float64>);
    template <>
    constexpr float32 log2e_v<float32> = static_cast<float32>(log2e_v<float64>);
    template <>
    constexpr float32 log10e_v<float32> = static_cast<float32>(log10e_v<float64>);
    template <>
    constexpr float32 pi_v<float32> = static_cast<float32>(pi_v<float64>);
    template <>
    constexpr float32 log2pi_v<float32> = static_cast<float32>(log2pi_v<float64>); /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <>
    constexpr float32 log_sqrt_2pi_v<float32> = static_cast<float32>(log_sqrt_2pi_v<float64>); /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <>
    constexpr float32 half_pi_v<float32> = static_cast<float32>(half_pi_v<float64>); /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <>
    constexpr float32 sqrt_pi_v<float32> = static_cast<float32>(sqrt_pi_v<float64>); /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <>
    constexpr float32 sqrt_half_pi_v<float32> = static_cast<float32>(sqrt_half_pi_v<float64>); /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <>
    constexpr float32 inv_pi_v<float32> = static_cast<float32>(inv_pi_v<float64>);
    template <>
    constexpr float32 inv_sqrtpi_v<float32> = static_cast<float32>(inv_sqrtpi_v<float64>);
    template <>
    constexpr float32 ln2_v<float32> = static_cast<float32>(ln2_v<float64>);
    template <>
    constexpr float32 ln10_v<float32> = static_cast<float32>(ln10_v<float64>);
    template <>
    constexpr float32 sqrt2_v<float32> = static_cast<float32>(sqrt2_v<float64>);
    template <>
    constexpr float32 sqrt3_v<float32> = static_cast<float32>(sqrt3_v<float64>);
    template <>
    constexpr float32 inv_sqrt3_v<float32> = static_cast<float32>(inv_sqrt3_v<float64>);
    template <>
    constexpr float32 egamma_v<float32> = static_cast<float32>(egamma_v<float64>);
    template <>
    constexpr float32 phi_v<float32> = static_cast<float32>(phi_v<float64>);

    template <>
    constexpr lfloat64 e_v<lfloat64> = e_v<float64>;
    template <>
    constexpr lfloat64 log2e_v<lfloat64> = log2e_v<float64>;
    template <>
    constexpr lfloat64 log10e_v<lfloat64> = log10e_v<float64>;
    template <>
    constexpr lfloat64 pi_v<lfloat64> = pi_v<float64>;
    template <>
    constexpr lfloat64 log2pi_v<lfloat64> = static_cast<lfloat64>(log2pi_v<float64>); /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <>
    constexpr lfloat64 log_sqrt_2pi_v<lfloat64> = static_cast<lfloat64>(log_sqrt_2pi_v<float64>); /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <>
    constexpr lfloat64 half_pi_v<lfloat64> = static_cast<lfloat64>(half_pi_v<float64>); /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <>
    constexpr lfloat64 sqrt_pi_v<lfloat64> = static_cast<lfloat64>(sqrt_pi_v<float64>); /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <>
    constexpr lfloat64 sqrt_half_pi_v<lfloat64> = static_cast<lfloat64>(sqrt_half_pi_v<float64>); /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    template <>
    constexpr lfloat64 inv_pi_v<lfloat64> = inv_pi_v<float64>;
    template <>
    constexpr lfloat64 inv_sqrtpi_v<lfloat64> = inv_sqrtpi_v<float64>;
    template <>
    constexpr lfloat64 ln2_v<lfloat64> = ln2_v<float64>;
    template <>
    constexpr lfloat64 ln10_v<lfloat64> = ln10_v<float64>;
    template <>
    constexpr lfloat64 sqrt2_v<lfloat64> = sqrt2_v<float64>;
    template <>
    constexpr lfloat64 sqrt3_v<lfloat64> = sqrt3_v<float64>;
    template <>
    constexpr lfloat64 inv_sqrt3_v<lfloat64> = inv_sqrt3_v<float64>;
    template <>
    constexpr lfloat64 egamma_v<lfloat64> = egamma_v<float64>;
    template <>
    constexpr lfloat64 phi_v<lfloat64> = phi_v<float64>;

    constexpr float32 E          = e_v<float32>;
    constexpr float32 Log2E      = log2e_v<float32>;
    constexpr float32 Log10E     = log10e_v<float32>;
    constexpr float32 Pi         = pi_v<float32>;
    constexpr float32 Log2Pi     = log2pi_v<float32>;       /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    constexpr float32 LogSqrt2Pi = log_sqrt_2pi_v<float32>; /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    constexpr float32 HalfPi     = half_pi_v<float32>;      /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    constexpr float32 SqrtPi     = sqrt_pi_v<float32>;      /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    constexpr float32 SqrtHalfPi = sqrt_half_pi_v<float32>; /// RSL Comment: Not in ISO C++ Standard at time of writing (18/Aug/2022)
    constexpr float32 InvPi      = inv_pi_v<float32>;
    constexpr float32 InvSqrtPi  = inv_sqrtpi_v<float32>;
    constexpr float32 Ln2        = ln2_v<float32>;
    constexpr float32 Ln10       = ln10_v<float32>;
    constexpr float32 Sqrt2      = sqrt2_v<float32>;
    constexpr float32 Sqrt3      = sqrt3_v<float32>;
    constexpr float32 InvSqrt3   = inv_sqrt3_v<float32>;
    constexpr float32 EGamma     = egamma_v<float32>;
    constexpr float32 Phi        = phi_v<float32>;

  } // namespace v1
} // namespace rsl

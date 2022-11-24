// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: constants.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {
    namespace limits_byte
    {
      inline constexpr card32 num_bits_per_byte = 8;
    } // namespace limits_byte

    namespace limits_8bit
    {
      inline constexpr int32 signed_min    = -128;
      inline constexpr card32 signed_max   = 127;
      inline constexpr card32 unsigned_max = 255;
    } // namespace limits_8bit

    namespace limits_16bit
    {
      inline constexpr int32 signed_min    = -32'768;
      inline constexpr card32 signed_max   = 32'767;
      inline constexpr card32 unsigned_max = 65'535;
    } // namespace limits_16bit

    namespace limits_32bit
    {
      inline constexpr int32 signed_min    = -2'147'483'647 - 1;
      inline constexpr int32 signed_max    = 2'147'483'647;
      inline constexpr uint32 unsigned_max = 4'294'967'295;

      inline constexpr card32 num_rounding_digits   = 9;
      inline constexpr card32 num_precision_digits  = 6;
      inline constexpr float32 epsilon_value        = 1.192092896e-07f;
      inline constexpr bool supports_sub_normal     = true;
      inline constexpr card32 num_mantissa_bits     = 24;
      inline constexpr float32 float_min_normalized = 1.175494351e-38F;
      inline constexpr float32 float_min            = 1.401298464e-45F;
      inline constexpr float32 float_max            = 3.402823466e+38F;
      inline constexpr card32 max_decimal_exponent  = 38;
      inline constexpr card32 max_binary_exponent   = 128;
      inline constexpr card32 min_decimal_exponent  = -37;
      inline constexpr card32 min_binary_exponent   = -125;
      inline constexpr card32 exponent_radix        = 2;
    } // namespace limits_32bit

    namespace limits_64bit
    {
      inline constexpr int64 signed_min    = (-9'223'372'036'854'775'807) - 1;
      inline constexpr int64 signed_max    = 9'223'372'036'854'775'807;
      inline constexpr uint64 unsigned_max = 18'446'744'073'709'551'615ui64;

      inline constexpr card32 num_rounding_digits   = 17;
      inline constexpr card32 num_precision_digits  = 15;
      inline constexpr float64 epsilon_value        = 2.2204460492503131e-016;
      inline constexpr bool supports_sub_normal     = true;
      inline constexpr card32 num_mantissa_bits     = 53;
      inline constexpr float64 float_min_normalized = 4.9406564584124654e-324;
      inline constexpr float64 float_min            = 2.2250738585072014e-30;
      inline constexpr float64 float_max            = 1.7976931348623158e+308;
      inline constexpr card32 max_decimal_exponent  = 308;
      inline constexpr card32 max_binary_exponent   = 1024;
      inline constexpr card32 min_decimal_exponent  = -307;
      inline constexpr card32 min_binary_exponent   = -1021;
      inline constexpr card32 exponent_radix        = 2;
    } // namespace limits_64bit
  }   // namespace v1
} // namespace rsl

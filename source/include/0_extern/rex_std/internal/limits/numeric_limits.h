// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: numeric_limits.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/attributes.h"
#include "rex_std/bonus/types.h"
#include "rex_std/internal/limits/constants.h"
#include "rex_std/type_traits.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      // ENUM FloatDenormStyle
      // constants for different IEEE float denormalization
      // styles
      enum class float_denorm_style
      {
        denorm_indeterminate = -1,
        denorm_absent        = 0,
        denorm_present       = 1
      };

      // ENUM FloatRoundStyle
      // constants for different IEEE rounding styles
      enum class float_round_style
      {
        round_indeterminate       = -1,
        round_toward_zero         = 0,
        round_to_nearest          = 1,
        round_toward_infinity     = 2,
        round_toward_neg_infinity = 3
      };

      // STRUCT NumBase
      struct num_base
      { // base for all types, with common defaults
        static constexpr float_denorm_style has_denorm = float_denorm_style::denorm_absent;
        static constexpr bool has_denorm_loss          = false;
        static constexpr bool has_infinity             = false;
        static constexpr bool has_quiet_nan            = false;
        static constexpr bool has_signaling_nan        = false;
        static constexpr bool is_bounded               = false;
        static constexpr bool is_exact                 = false;
        static constexpr bool is_iec559                = false;
        static constexpr bool is_integer               = false;
        static constexpr bool is_modulo                = false;
        static constexpr bool is_signed                = false;
        static constexpr bool is_specialized           = false;
        static constexpr bool tinyness_before          = false;
        static constexpr bool trap                     = false;
        static constexpr float_round_style round_style = float_round_style::round_toward_zero;
        static constexpr int32 digits                  = 0;
        static constexpr int32 digits10                = 0;
        static constexpr int32 max_digits10            = 0;
        static constexpr int32 max_exponent            = 0;
        static constexpr int32 max_exponent10          = 0;
        static constexpr int32 min_exponent            = 0;
        static constexpr int32 min_exponent10          = 0;
        static constexpr int32 radix                   = 0;
      };

      // STRUCT NumIntBase
      struct num_int_base : num_base
      { // base for integer types
        static constexpr bool is_bounded     = true;
        static constexpr bool is_exact       = true;
        static constexpr bool is_integer     = true;
        static constexpr bool is_specialized = true;
        static constexpr int32 radix         = 2;
      };

      // STRUCT NumFloatBase
      struct num_float_base : num_base
      { // base for floating-point types
        static constexpr float_denorm_style has_denorm = float_denorm_style::denorm_present;
        static constexpr bool has_infinity             = true;
        static constexpr bool has_quiet_nan            = true;
        static constexpr bool has_signaling_nan        = true;
        static constexpr bool is_bounded               = true;
        static constexpr bool is_iec559                = true;
        static constexpr bool is_signed                = true;
        static constexpr bool is_specialized           = true;
        static constexpr float_round_style round_style = float_round_style::round_to_nearest;
        static constexpr int32 radix                   = limits_32bit::exponent_radix;
      };

      // the number of bits in the represenation of T
      template <typename T>
      inline constexpr auto nr_of_digits_signed = (sizeof(T) * limits_byte::num_bits_per_byte) - 1;
      template <typename T>
      inline constexpr auto nr_of_digits_unsigned = (sizeof(T) * limits_byte::num_bits_per_byte);
      template <typename T>
      inline constexpr auto nr_of_digits = rsl::is_signed_v<T> ? nr_of_digits_signed<T> : nr_of_digits_unsigned<T>;

      // the min possible value of T
      template <typename T>
      inline constexpr auto min_signed = static_cast<T>(1) << nr_of_digits<T>;
      template <typename T>
      inline constexpr auto min_unsigned = static_cast<T>(0);
      template <typename T>
      inline constexpr auto min = rsl::is_signed_v<T> ? min_signed<T> : min_unsigned<T>;

      // the max possible value of T
      template <typename T>
      inline constexpr auto max_signed = static_cast<T>((((static_cast<T>(1) << (nr_of_digits<T> - 1)) - 1) << 1) + 1);
      template <typename T>
      inline constexpr auto max_unsigned = (static_cast<T>(~static_cast<T>(0)));
      template <typename T>
      inline constexpr auto max = rsl::is_signed_v<T> ? max_signed<T> : max_unsigned<T>;

      // the number of decimal digits that can be represented by T
      template <typename T>
      inline constexpr auto nr_of_digits10_signed = (nr_of_digits_signed<T> * 643L) / 2136; // ( 643 / 2136 ) ~= log10(2)
      template <typename T>
      inline constexpr auto nr_of_digits10_unsigned = (nr_of_digits_unsigned<T> * 643L) / 2136; // ( 643 / 2136 ) ~= log10(2)
      template <typename T>
      inline constexpr auto nr_of_digits10 = rsl::is_signed_v<T> ? nr_of_digits10_signed<T> : nr_of_digits10_unsigned<T>;
    } // namespace internal

    // CLASS TEMPLATE numeric_limits
    template <class T>
    class numeric_limits : public internal::num_base
    { // numeric limits for arbitrary type
      // T (say little or nothing)
    public:
      REX_NO_DISCARD static constexpr T(min)() noexcept
      {
        return T();
      }

      REX_NO_DISCARD static constexpr T(max)() noexcept
      {
        return T();
      }

      REX_NO_DISCARD static constexpr T lowest() noexcept
      {
        return T();
      }

      REX_NO_DISCARD static constexpr T epsilon() noexcept
      {
        return T();
      }

      REX_NO_DISCARD static constexpr T round_error() noexcept
      {
        return T();
      }

      REX_NO_DISCARD static constexpr T denorm_min() noexcept
      {
        return T();
      }

      REX_NO_DISCARD static constexpr T infinity() noexcept
      {
        return T();
      }

      REX_NO_DISCARD static constexpr T quiet_na_n() noexcept
      {
        return T();
      }

      REX_NO_DISCARD static constexpr T signaling_na_n() noexcept
      {
        return T();
      }
    };

    template <class T>
    class numeric_limits<const T> : public numeric_limits<T>
    {
    }; // numeric limits for const types

    template <class T>
    class numeric_limits<volatile T> : public numeric_limits<T>
    {
    }; // numeric limits for volatile types

    template <class T>
    class numeric_limits<const volatile T> : public numeric_limits<T>
    {
    }; // numeric limits for const volatile types

    // CLASS numeric_limits<bool>
    template <>
    class numeric_limits<bool> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr bool(min)() noexcept
      {
        return false;
      }

      REX_NO_DISCARD static constexpr bool(max)() noexcept
      {
        return true;
      }

      REX_NO_DISCARD static constexpr bool lowest() noexcept
      {
        return (min)();
      }

      REX_NO_DISCARD static constexpr bool epsilon() noexcept
      {
        return false;
      }

      REX_NO_DISCARD static constexpr bool round_error() noexcept
      {
        return false;
      }

      REX_NO_DISCARD static constexpr bool denorm_min() noexcept
      {
        return false;
      }

      REX_NO_DISCARD static constexpr bool infinity() noexcept
      {
        return false;
      }

      REX_NO_DISCARD static constexpr bool quiet_na_n() noexcept
      {
        return false;
      }

      REX_NO_DISCARD static constexpr bool signaling_na_n() noexcept
      {
        return false;
      }

      static constexpr int32 digits = 1;
    };

    // CLASS numeric_limits<char>
    template <>
    class numeric_limits<char> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr char(min)() noexcept
      {
        return limits_8bit::signed_min;
      }

      REX_NO_DISCARD static constexpr char(max)() noexcept
      {
        return limits_8bit::signed_max;
      }

      REX_NO_DISCARD static constexpr char lowest() noexcept
      {
        return (min)();
      }

      REX_NO_DISCARD static constexpr char epsilon() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char round_error() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char denorm_min() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char infinity() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char quiet_na_n() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char signaling_na_n() noexcept
      {
        return 0;
      }

      static constexpr bool is_signed = limits_8bit::signed_min != 0;
      static constexpr bool is_modulo = limits_8bit::signed_min == 0;
      static constexpr int32 digits   = 8 - static_cast<int32>((limits_8bit::signed_min != 0));
      static constexpr int32 digits10 = 2;
    };

    // CLASS numeric_limits<signed char>
    template <>
    class numeric_limits<signed char> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr signed char(min)() noexcept
      {
        return limits_8bit::signed_min;
      }

      REX_NO_DISCARD static constexpr signed char(max)() noexcept
      {
        return limits_8bit::signed_max;
      }

      REX_NO_DISCARD static constexpr signed char lowest() noexcept
      {
        return (min)();
      }

      REX_NO_DISCARD static constexpr signed char epsilon() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr signed char round_error() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr signed char denorm_min() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr signed char infinity() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr signed char quiet_na_n() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr signed char signaling_na_n() noexcept
      {
        return 0;
      }

      static constexpr bool is_signed = true;
      static constexpr int32 digits   = 7;
      static constexpr int32 digits10 = 2;
    };

    // CLASS numeric_limits<unsigned char>
    template <>
    class numeric_limits<unsigned char> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr unsigned char(min)() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned char(max)() noexcept
      {
        return limits_8bit::unsigned_max;
      }

      REX_NO_DISCARD static constexpr unsigned char lowest() noexcept
      {
        return (min)();
      }

      REX_NO_DISCARD static constexpr unsigned char epsilon() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned char round_error() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned char denorm_min() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned char infinity() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned char quiet_na_n() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned char signaling_na_n() noexcept
      {
        return 0;
      }

      static constexpr bool is_modulo = true;
      static constexpr int32 digits   = 8;
      static constexpr int32 digits10 = 2;
    };

#ifdef __cpp_char8_t
    // CLASS numeric_limits<char8_t>
    template <>
    class numeric_limits<char8_t> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr char8_t(min)() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char8_t(max)() noexcept
      {
        return limits_8bit::unsigned_max;
      }

      REX_NO_DISCARD static constexpr char8_t lowest() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char8_t epsilon() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char8_t round_error() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char8_t denorm_min() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char8_t infinity() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char8_t quiet_na_n() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char8_t signaling_na_n() noexcept
      {
        return 0;
      }

      static constexpr bool is_modulo = true;
      static constexpr int32 digits   = 8;
      static constexpr int32 digits10 = 2;
    };
#endif // __cpp_char8_t

    // CLASS numeric_limits<char16_t>
    template <>
    class numeric_limits<char16_t> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr char16_t(min)() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char16_t(max)() noexcept
      {
        return limits_16bit::unsigned_max;
      }

      REX_NO_DISCARD static constexpr char16_t lowest() noexcept
      {
        return (min)();
      }

      REX_NO_DISCARD static constexpr char16_t epsilon() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char16_t round_error() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char16_t denorm_min() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char16_t infinity() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char16_t quiet_na_n() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char16_t signaling_na_n() noexcept
      {
        return 0;
      }

      static constexpr bool is_modulo = true;
      static constexpr int32 digits   = 16;
      static constexpr int32 digits10 = 4;
    };

    // CLASS numeric_limits<char32_t>
    template <>
    class numeric_limits<char32_t> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr char32_t(min)() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char32_t(max)() noexcept
      {
        return limits_32bit::unsigned_max;
      }

      REX_NO_DISCARD static constexpr char32_t lowest() noexcept
      {
        return (min)();
      }

      REX_NO_DISCARD static constexpr char32_t epsilon() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char32_t round_error() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char32_t denorm_min() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char32_t infinity() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char32_t quiet_na_n() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr char32_t signaling_na_n() noexcept
      {
        return 0;
      }

      static constexpr bool is_modulo = true;
      static constexpr int32 digits   = 32;
      static constexpr int32 digits10 = 9;
    };

    // CLASS numeric_limits<wchar_t>
    template <>
    class numeric_limits<wchar_t> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr wchar_t(min)() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr wchar_t(max)() noexcept
      {
#ifdef _MSC_VER
        return limits_16bit::unsigned_max;
#else
        return limits_32bit::UnsignedMax;
#endif
      }

      REX_NO_DISCARD static constexpr wchar_t lowest() noexcept
      {
        return (min)();
      }

      REX_NO_DISCARD static constexpr wchar_t epsilon() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr wchar_t round_error() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr wchar_t denorm_min() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr wchar_t infinity() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr wchar_t quiet_na_n() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr wchar_t signaling_na_n() noexcept
      {
        return 0;
      }

      static constexpr bool is_modulo = true;
      static constexpr int32 digits   = 16;
      static constexpr int32 digits10 = 4;
    };

    // CLASS numeric_limits<short>
    template <>
    class numeric_limits<short> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr short(min)() noexcept
      {
        return limits_16bit::signed_min;
      }

      REX_NO_DISCARD static constexpr short(max)() noexcept
      {
        return limits_16bit::signed_max;
      }

      REX_NO_DISCARD static constexpr short lowest() noexcept
      {
        return (min)();
      }

      REX_NO_DISCARD static constexpr short epsilon() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr short round_error() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr short denorm_min() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr short infinity() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr short quiet_na_n() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr short signaling_na_n() noexcept
      {
        return 0;
      }

      static constexpr bool is_signed = true;
      static constexpr int32 digits   = 15;
      static constexpr int32 digits10 = 4;
    };

    // CLASS numeric_limits<int32>
    template <>
    class numeric_limits<int32> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr int32(min)() noexcept
      {
        return limits_32bit::signed_min;
      }

      REX_NO_DISCARD static constexpr int32(max)() noexcept
      {
        return limits_32bit::signed_max;
      }

      REX_NO_DISCARD static constexpr int32 lowest() noexcept
      {
        return (min)();
      }

      REX_NO_DISCARD static constexpr int32 epsilon() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr int32 round_error() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr int32 denorm_min() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr int32 infinity() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr int32 quiet_na_n() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr int32 signaling_na_n() noexcept
      {
        return 0;
      }

      static constexpr bool is_signed = true;
      static constexpr int32 digits   = 31;
      static constexpr int32 digits10 = 9;
    };

    // CLASS numeric_limits<long>
    template <>
    class numeric_limits<long> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr long(min)() noexcept
      {
        return limits_32bit::signed_min;
      }

      REX_NO_DISCARD static constexpr long(max)() noexcept
      {
        return limits_32bit::signed_max;
      }

      REX_NO_DISCARD static constexpr long lowest() noexcept
      {
        return (min)();
      }

      REX_NO_DISCARD static constexpr long epsilon() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr long round_error() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr long denorm_min() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr long infinity() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr long quiet_na_n() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr long signaling_na_n() noexcept
      {
        return 0;
      }

      static_assert(sizeof(int32) == sizeof(long), "LongLP64 assumption");
      static constexpr bool is_signed = true;
      static constexpr int32 digits   = 31;
      static constexpr int32 digits10 = 9;
    };

    // CLASS numeric_limits<long long>
    template <>
    class numeric_limits<long long> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr long long(min)() noexcept
      {
        return limits_64bit::signed_min;
      }

      REX_NO_DISCARD static constexpr long long(max)() noexcept
      {
        return limits_64bit::signed_max;
      }

      REX_NO_DISCARD static constexpr long long lowest() noexcept
      {
        return (min)();
      }

      REX_NO_DISCARD static constexpr long long epsilon() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr long long round_error() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr long long denorm_min() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr long long infinity() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr long long quiet_na_n() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr long long signaling_na_n() noexcept
      {
        return 0;
      }

      static constexpr bool is_signed = true;
      static constexpr int32 digits   = 63;
      static constexpr int32 digits10 = 18;
    };

#ifdef _NATIVE_WCHAR_T_DEFINED
    // CLASS numeric_limits<unsigned short>
    template <>
    class numeric_limits<unsigned short> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr unsigned short(min)() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned short(max)() noexcept
      {
        return limits_16bit::unsigned_max;
      }

      REX_NO_DISCARD static constexpr unsigned short lowest() noexcept
      {
        return (min)();
      }

      REX_NO_DISCARD static constexpr unsigned short epsilon() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned short round_error() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned short denorm_min() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned short infinity() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned short quiet_na_n() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned short signaling_na_n() noexcept
      {
        return 0;
      }

      static constexpr bool is_modulo = true;
      static constexpr int32 digits   = 16;
      static constexpr int32 digits10 = 4;
    };
#endif // _NATIVE_WCHAR_T_DEFINED

    // CLASS numeric_limits<uint32>
    template <>
    class numeric_limits<uint32> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr uint32(min)() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr uint32(max)() noexcept
      {
        return limits_32bit::unsigned_max;
      }

      REX_NO_DISCARD static constexpr uint32 lowest() noexcept
      {
        return (min)();
      }

      REX_NO_DISCARD static constexpr uint32 epsilon() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr uint32 round_error() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr uint32 denorm_min() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr uint32 infinity() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr uint32 quiet_na_n() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr uint32 signaling_na_n() noexcept
      {
        return 0;
      }

      static constexpr bool is_modulo = true;
      static constexpr int32 digits   = 32;
      static constexpr int32 digits10 = 9;
    };

    // CLASS numeric_limits<unsigned long>
    template <>
    class numeric_limits<unsigned long> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr unsigned long(min)() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned long(max)() noexcept
      {
        return limits_32bit::unsigned_max;
      }

      REX_NO_DISCARD static constexpr unsigned long lowest() noexcept
      {
        return (min)();
      }

      REX_NO_DISCARD static constexpr unsigned long epsilon() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned long round_error() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned long denorm_min() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned long infinity() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned long quiet_na_n() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned long signaling_na_n() noexcept
      {
        return 0;
      }

      static_assert(sizeof(uint32) == sizeof(unsigned long), "LongLP64 assumption");
      static constexpr bool is_modulo = true;
      static constexpr int32 digits   = 32;
      static constexpr int32 digits10 = 9;
    };

    // CLASS numeric_limits<unsigned long long>
    template <>
    class numeric_limits<unsigned long long> : public internal::num_int_base
    {
    public:
      REX_NO_DISCARD static constexpr unsigned long long(min)() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned long long(max)() noexcept
      {
        return limits_64bit::unsigned_max;
      }

      REX_NO_DISCARD static constexpr unsigned long long lowest() noexcept
      {
        return (min)();
      }

      REX_NO_DISCARD static constexpr unsigned long long epsilon() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned long long round_error() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned long long denorm_min() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned long long infinity() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned long long quiet_na_n() noexcept
      {
        return 0;
      }

      REX_NO_DISCARD static constexpr unsigned long long signaling_na_n() noexcept
      {
        return 0;
      }

      static constexpr bool is_modulo = true;
      static constexpr int32 digits   = 64;
      static constexpr int32 digits10 = 19;
    };

    // CLASS numeric_limits<float>
    template <>
    class numeric_limits<float> : public internal::num_float_base
    {
    public:
      REX_NO_DISCARD static constexpr float(min)() noexcept
      {
        return limits_32bit::float_min_normalized;
      }

      REX_NO_DISCARD static constexpr float(max)() noexcept
      {
        return limits_32bit::float_max;
      }

      REX_NO_DISCARD static constexpr float lowest() noexcept
      {
        return -(max)();
      }

      REX_NO_DISCARD static constexpr float epsilon() noexcept
      {
        return limits_32bit::epsilon_value;
      }

      REX_NO_DISCARD static constexpr float round_error() noexcept
      {
        return 0.5F;
      }

      REX_NO_DISCARD static constexpr float denorm_min() noexcept
      {
        return limits_32bit::float_min;
      }

      REX_NO_DISCARD static constexpr float infinity() noexcept
      {
        return __builtin_huge_valf();
      }

      REX_NO_DISCARD static constexpr float quiet_na_n() noexcept
      {
        return __builtin_nanf("0");
      }

      REX_NO_DISCARD static constexpr float signaling_na_n() noexcept
      {
        return __builtin_nansf("1");
      }

      static constexpr int32 digits         = limits_32bit::num_mantissa_bits;
      static constexpr int32 digits10       = limits_32bit::num_precision_digits;
      static constexpr int32 max_digits10   = 9;
      static constexpr int32 max_exponent   = limits_32bit::max_binary_exponent;
      static constexpr int32 max_exponent10 = limits_32bit::max_decimal_exponent;
      static constexpr int32 min_exponent   = limits_32bit::min_binary_exponent;
      static constexpr int32 min_exponent10 = limits_32bit::min_decimal_exponent;
    };

    // CLASS numeric_limits<double>
    template <>
    class numeric_limits<double> : public internal::num_float_base
    {
    public:
      REX_NO_DISCARD static constexpr double(min)() noexcept
      {
        return limits_64bit::float_min_normalized;
      }

      REX_NO_DISCARD static constexpr double(max)() noexcept
      {
        return limits_64bit::float_max;
      }

      REX_NO_DISCARD static constexpr double lowest() noexcept
      {
        return -(max)();
      }

      REX_NO_DISCARD static constexpr double epsilon() noexcept
      {
        return limits_64bit::epsilon_value;
      }

      REX_NO_DISCARD static constexpr double round_error() noexcept
      {
        return 0.5;
      }

      REX_NO_DISCARD static constexpr double denorm_min() noexcept
      {
        return limits_64bit::float_min;
      }

      REX_NO_DISCARD static constexpr double infinity() noexcept
      {
        return __builtin_huge_val();
      }

      REX_NO_DISCARD static constexpr double quiet_na_n() noexcept
      {
        return __builtin_nan("0");
      }

      REX_NO_DISCARD static constexpr double signaling_na_n() noexcept
      {
        return __builtin_nans("1");
      }

      static constexpr int32 digits         = limits_64bit::num_mantissa_bits;
      static constexpr int32 digits10       = limits_64bit::num_precision_digits;
      static constexpr int32 max_digits10   = 17;
      static constexpr int32 max_exponent   = limits_64bit::max_binary_exponent;
      static constexpr int32 max_exponent10 = limits_64bit::max_decimal_exponent;
      static constexpr int32 min_exponent   = limits_64bit::min_binary_exponent;
      static constexpr int32 min_exponent10 = limits_64bit::min_decimal_exponent;
    };

    // CLASS numeric_limits<long double>
    template <>
    class numeric_limits<long double> : public internal::num_float_base
    {
    public:
      REX_NO_DISCARD static constexpr long double(min)() noexcept
      {
        return limits_64bit::float_min_normalized;
      }

      REX_NO_DISCARD static constexpr long double(max)() noexcept
      {
        return limits_64bit::float_max;
      }

      REX_NO_DISCARD static constexpr long double lowest() noexcept
      {
        return -(max)();
      }

      REX_NO_DISCARD static constexpr long double epsilon() noexcept
      {
        return limits_64bit::epsilon_value;
      }

      REX_NO_DISCARD static constexpr long double round_error() noexcept
      {
        return 0.5L;
      }

      REX_NO_DISCARD static constexpr long double denorm_min() noexcept
      {
        return limits_64bit::float_min;
      }

      REX_NO_DISCARD static constexpr long double infinity() noexcept
      {
        return __builtin_huge_val(); // NOLINT(cppcoreguidelines-narrowing-conversions)
      }

      REX_NO_DISCARD static constexpr long double quiet_na_n() noexcept
      {
        return __builtin_nan("0");
      }

      REX_NO_DISCARD static constexpr long double signaling_na_n() noexcept
      {
        return __builtin_nans("1");
      }

      static constexpr int32 digits         = limits_64bit::num_mantissa_bits;
      static constexpr int32 digits10       = limits_64bit::num_precision_digits;
      static constexpr int32 max_digits10   = 17;
      static constexpr int32 max_exponent   = limits_64bit::max_binary_exponent;
      static constexpr int32 max_exponent10 = limits_64bit::max_decimal_exponent;
      static constexpr int32 min_exponent   = limits_64bit::min_binary_exponent;
      static constexpr int32 min_exponent10 = limits_64bit::min_decimal_exponent;
    };

  } // namespace v1
} // namespace rsl

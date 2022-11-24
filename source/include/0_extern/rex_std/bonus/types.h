// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: types.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/bonus/utility/strong_type.h"

namespace rsl
{
  inline namespace v1
  {
    // integers

    using int8  = signed char;
    using int16 = signed short;
    using int32 = signed int;
    using int64 = signed long long;

    static_assert(sizeof(int8) == 1, "int8 must be 1 byte big");    // NOLINT
    static_assert(sizeof(int16) == 2, "int16 must be 2 bytes big"); // NOLINT
    static_assert(sizeof(int32) == 4, "int32 must be 4 bytes big"); // NOLINT
    static_assert(sizeof(int64) == 8, "int64 must be 8 bytes big"); // NOLINT

    using uint8  = unsigned char;
    using uint16 = unsigned short;
    using uint32 = unsigned int;
    using uint64 = unsigned long long;

    static_assert(sizeof(uint8) == 1, "uint8 must be 1 byte big");    // NOLINT
    static_assert(sizeof(uint16) == 2, "uint16 must be 2 bytes big"); // NOLINT
    static_assert(sizeof(uint32) == 4, "uint32 must be 4 bytes big"); // NOLINT
    static_assert(sizeof(uint64) == 8, "uint64 must be 8 bytes big"); // NOLINT

    using ulong = unsigned long;

    // long is 64 bits on Linux and macOS
    // use case of long is highly discouraged
    // due to their inconsistent size on different platforms
    static_assert(sizeof(long) == sizeof(int32), "long should be the same size as int32");
    static_assert(sizeof(ulong) == sizeof(uint32), "ulong should be the same size as uint32");

    // Yes, cardinals are meant to always be positive,
    // so it makes sense for them to be unsigned.
    // however, because we don't want to use unsigned types
    // because they're such a pain,
    // cardinals are signed, but their value should always be positive!
    using card8  = int8;
    using card16 = int16;
    using card32 = int32;
    using card64 = int64;

    // characters
    using char8  = char;
    using char16 = char16_t;
    using char32 = char32_t;
    using tchar  = wchar_t;

    static_assert(sizeof(char8) == 1, "char8 must be 1 byte big");   // NOLINT
    static_assert(sizeof(char16) == 2, "char16 must be 2 byte big"); // NOLINT
    static_assert(sizeof(char32) == 4, "char32 must be 4 byte big"); // NOLINT

// with MSVC, wchar_t is 2 bytes big, while with clang and gcc, it's 4 bytes big
#ifdef _MSC_VER
    static_assert(sizeof(tchar) == 2, "tchar must be 2 bytes big");
#else
    static_assert(sizeof(tchar) == 4, "tchar must be 4 bytes big");
#endif

    // floating-point
    using float32 = float;
    using float64 = double;

    static_assert(sizeof(float32) == 4, "float32 must be 4 bytes big"); // NOLINT
    static_assert(sizeof(float64) == 8, "float64 must be 8 bytes big"); // NOLINT

    using lfloat64 = long double;
    static_assert(sizeof(lfloat64) == 8, "long float must be 8 bytes big"); // NOLINT

    // bool
    static_assert(sizeof(bool) == 1, "bool must be 1 byte big");

    // enums

    namespace internal
    {
      // unscoped enums have an implementation defined underlying type not larger than int
      // this means it can be smaller, which could cause some horrible bugs when reading/writing data
      // on different platforms
      enum test_enum
      {
      };
      static_assert(sizeof(test_enum) == sizeof(int32), "unscoped enum must be 4 bytes big");

      // scoped enums default to underlying type is int, so let's make sure it actually is
      enum class test_scoped_enum
      {
      };
      static_assert(sizeof(test_scoped_enum) == sizeof(int32), "scoped enum must be 4 bytes big");
    } // namespace internal

    static_assert(false == 0, "C++ false keyword does not equal 0"); // NOLINT
    static_assert(true == 1, "C++ true keyword does not equal 1");   // NOLINT

// types that need to be defined for standard purposes
#if defined(REX_PLATFORM_X86)
    using intptr  = int32;
    using uintptr = uint32;
    using ptrdiff = int32;
    using intmax  = int32;
    using uintmax = uint32;
    using size_t  = uint32;
#elif defined(REX_PLATFORM_X64)
    using intptr  = int64;
    using uintptr = uint64;
    using ptrdiff = int64;
    using intmax  = int64;
    using uintmax = uint64;
    using size_t  = uint64;
#else
  #error "No platform specified"
#endif

    // this is RSL's version of size_t.
    // size_t is meant to be used with sizeof() as it's meant to be able to hold
    // all values possibly returned by sizeof().
    // However, the standard started using it for size of a container, index types, etc..
    // This is not done in RSL, we use count_t for this.
    // The reason we use count_t and not card32 directly is so we can change
    // this type in the future, should it be necessary.
    using count_t = card32;

    // useful strong types
    REX_CREATE_STRONG_TYPE(Capacity, card32);
    REX_CREATE_STRONG_TYPE(Size, card32);

  } // namespace v1
} // namespace rsl

REX_CREATE_LITERAL_FOR_STRONG_TYPE(rsl::Capacity, cap)
REX_CREATE_LITERAL_FOR_STRONG_TYPE(rsl::Size, size)

using rsl::int16; // NOLINT
using rsl::int32; // NOLINT
using rsl::int64; // NOLINT
using rsl::int8;  // NOLINT

using rsl::uint16; // NOLINT
using rsl::uint32; // NOLINT
using rsl::uint64; // NOLINT
using rsl::uint8;  // NOLINT

using rsl::card16; // NOLINT
using rsl::card32; // NOLINT
using rsl::card64; // NOLINT
using rsl::card8;  // NOLINT

using rsl::char8; // NOLINT
using rsl::tchar; // NOLINT

using rsl::float32; // NOLINT
using rsl::float64; // NOLINT

using rsl::intmax;  // NOLINT
using rsl::intptr;  // NOLINT
using rsl::ptrdiff; // NOLINT
using rsl::size_t;  // NOLINT
using rsl::uintptr; // NOLINT

using rsl::count_t; // NOLINT

REX_CREATE_LITERAL_FOR_STRONG_TYPE(rsl::Capacity, _cap);
REX_CREATE_LITERAL_FOR_STRONG_TYPE(rsl::Size, _size);

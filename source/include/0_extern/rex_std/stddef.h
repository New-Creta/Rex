// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: stddef.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/memory/byte.h"
#include "rex_std/internal/stddef/nullptr.h"
#include "rex_std/internal/type_traits/enable_if.h"
#include "rex_std/internal/type_traits/is_integral.h"

namespace rsl
{
  inline namespace v1
  {

// null pointer constant
#define NULL 0
    // byte offset from the beginning of a standard-layout type to specific member
    // #ifndef offsetof // it's possible due to some include that offsetof is already defined
    //     #define offsetof(s,m) ((size_t)&reinterpret_cast<char const volatile&>((((s*)0)->m)))
    // #endif

#if defined(REX_PLATFORM_X86)
    // unsigned integer type returned by the sizeof operator
    using size_t = uint32;
    // signed integer type returned when subtracting two pointers
    using ptrdiff = int32;
    // trivial type with alignment requirement as great as any other scalar type
    using max_align = float32;
#elif defined(REX_PLATFORM_X64)
    // unsigned integer type returned by the sizeof operator
    using size_t = uint64;
    // signed integer type returned when subtracting two pointers
    using ptrdiff = int64;
    // trivial type with alignment requirement as great as any other scalar type
    using max_align = float64;
#else
  #error "No platform defined"
#endif

  } // namespace v1
} // namespace rsl

#include "rex_std/internal/memory/byte.h"

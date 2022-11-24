// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: stdlib.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/cinttypes/div_result.h"
#include "rex_std/bonus/types.h"
#include "rex_std/internal/math/abs.h"
#include "rex_std/internal/string/byte_strings.h" // string to int functions are defined here

#include <cstdlib>

namespace rsl
{
  inline namespace v1
  {

// unsigned integer type returned by the sizeof operator
#if defined(REX_PLATFORM_X86)
    using size_t = uint32;
#elif defined(REX_PLATFORM_X64)
    using size_t = uint64;
#else
  #error "No platform defined"
#endif
// indicates the program ran successfully, used to pass to rsl::exit
#define EXIT_SUCCESS 0
// indicates the program failed, used to pass to rsl::exit
#define EXIT_FAILURE 1

// null pointer constant
#define NULL 0
// maximum return value of rsl::rand
#define RAND_MAX 0x7fff

    // The following functions can't be defined at the moment,
    // they are heavily platform specific, for now, just use the C standard ones
    // void abort();
    // void exit(int32 exit_code);
    // void quick_exit(int32 exit_code);
    // void _Exit(int32 exit_code);
    // int32 at_exit((void*)(int32) func);
    // int32 at_quick_exit((void*)(int32) func);
    // int32 system(const char8* command);
    // char8* getenv(const char8* env_var);

    // void* malloc(size_t size);
    // void* aligned_alloc(size_t alignment, size_t size);
    // void* calloc(size_t num, size_t size);
    // void free(void* ptr);

    /// RSL Comment: Different from ISO C++ Standard at time of writing (16/Jul/2022)
    // rsl is using count_t for n instead of size_t
    // returns the number of bytes in the next multibyte character
    int32 mblen(const char8* s, count_t n);

    /// RSL Comment: Different from ISO C++ Standard at time of writing (16/Jul/2022)
    // uses count_t for n instead of size_t
    // converts the next multibyte character to wide character
    int32 mbtowc(tchar* pwc, const char* s, count_t n);

    // converts a wide character to its multibyte representation
    int32 wctomb(char8* s, tchar wc);

    /// RSL Comment: Different from ISO C++ Standard at time of writing (16/Jul/2022)
    // uses count_t for len instead of size_t, also returns count_t instead of size_T
    // converts a narrow multibyte character string to a wide string
    count_t mbstowcs(tchar* dst, const char8* src, count_t len);

    /// RSL Comment: Different from ISO C++ Standard at time of writing (16/Jul/2022)
    // uses count_t for len instead of size_t, also returns count_t instead of size_T
    // converts a wide string to narrow multibyte character string
    count_t wcstombs(char8* dst, const tchar* src, count_t len);

    // generates a pseudo random number
    int32 rand();

    // seeds pseudo-random number generator
    void srand(uint32 seed);

    // sorts a range of elements with unspecified type
    void qsort(void* ptr, count_t count, size_t size, int32 (*)(const void*, const void*));

    // searches an array for an element of unspecified type
    void* bsearch(const void* key, const void* ptr, count_t count, size_t size, int32 (*)(const void*, const void*));

    // computes absolute value of an integral value
    int32 abs(int32 n)
    {
      return internal::abs(n);
    }
    // computes absolute value of an integral value
    long abs(long n)
    {
      return internal::abs(n);
    }
    // computes absolute value of an integral value
    int64 abs(int64 n)
    {
      return internal::abs(n);
    }
    // computes absolute value of an integral value
    long labs(long n)
    {
      return internal::abs(n);
    }
    // computes absolute value of an integral value
    int64 llabs(int64 n)
    {
      return internal::abs(n);
    }

    // type returned by rsl::div
    using div_t = DivResult<int32>;
    // type returns by rsl::ldiv
    using ldiv_t = DivResult<long>;
    // type returned by rsl::lldiv
    using lldiv_t = DivResult<int64>;
    // type returned by rsl::imaxdiv
    using imaxdiv_t = DivResult<intmax>;

    // computes quotient and remainder of integer division
    div_t div(int32 x, int32 y)
    {
      return rsl::div<int32>(x, y);
    }
    // computes quotient and remainder of integer division
    ldiv_t div(long x, long y)
    {
      return rsl::div<long>(x, y);
    }
    // computes quotient and remainder of integer division
    lldiv_t div(int64 x, int64 y)
    {
      return rsl::div<int64>(x, y);
    }
    // computes quotient and remainder of integer division
    ldiv_t ldiv(long x, long y)
    {
      return rsl::div<long>(x, y);
    }
    // computes quotient and remainder of integer division
    lldiv_t lldiv(int64 x, int64 y)
    {
      return rsl::div<int64>(x, y);
    }

  } // namespace v1
} // namespace rsl

#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <cstdlib>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_FUNC_ALIAS(abort);
    REX_STD_FUNC_ALIAS(exit);
    REX_STD_FUNC_ALIAS(quick_exit);
    REX_STD_FUNC_ALIAS(_Exit);
    REX_STD_FUNC_ALIAS(atexit);
    REX_STD_FUNC_ALIAS(at_quick_exit);
    REX_STD_FUNC_ALIAS(system);
    REX_STD_FUNC_ALIAS(getenv);
    REX_STD_FUNC_ALIAS(malloc);
    // REX_STD_FUNC_ALIAS(aligned_alloc); /// [29/Aug/2022] RSL Comment: not supported on MSVC
    REX_STD_FUNC_ALIAS(calloc);
    REX_STD_FUNC_ALIAS(realloc);
    REX_STD_FUNC_ALIAS(free);

    REX_STD_FUNC_ALIAS(mblen);
    REX_STD_FUNC_ALIAS(mbtowc);
    REX_STD_FUNC_ALIAS(wctomb);
    REX_STD_FUNC_ALIAS(mbstowcs);
    REX_STD_FUNC_ALIAS(wcstombs);
    REX_STD_FUNC_ALIAS(rand);
    REX_STD_FUNC_ALIAS(srand);
    REX_STD_FUNC_ALIAS(qsort);
    REX_STD_FUNC_ALIAS(bsearch);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: time.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {

// null pointer constant
#define NULL 0

    // type able to represent the process running time
    using clock_t = int32;

// unsigned integer type returned by the sizeof operator
#if defined(REX_PLATFORM_X86)
    using size_t = uint32;
#elif defined(REX_PLATFORM_X64)
    using size_t = uint64;
#else
  #error "No platform specified"
#endif
    // type able to represent times (usually seconds since 00:00, 1st Jan 1970)
    using time_t = uint64;

    /// RSL Comment: Different from ISO C++ Standard at time of writing (17/Jul/2022)
    // we're using bitfields here to reduce the size of tm from 36 bytes to 12 bytes (300% improvement)
    // it's yet to be determined if this is more optimized at runtime as the cpu now has to use bit masking
    // structure holding a calendar date and time broken down into its components
    struct tm
    {
      int32 tm_year;       // year since 1900
      int32 tm_yday : 10;  // days since january 1 [0, 365)
      int32 tm_sec : 7;    // seconds after the minute [0, 61)
      int32 tm_min : 7;    // minutes after the hour [0, 60)
      int32 tm_mday : 6;   // day of the month [1, 31)
      int32 tm_hour : 6;   // hours since midnight [0, 23)
      int32 tm_mon : 5;    // months since january [0, 11)
      int32 tm_wday : 4;   // day since sunday [0 , 6)
      int32 tm_isddst : 2; // is Daylight Saving Time in effect
    };

    // structure holding an interval broken down into seconds and nanoseconds
    struct timespec
    {
      time_t tv_sec;
      uint64 tv_nsec;
    };

    // returns the approximate processor time used by the process since the beginning of a program
    clock_t clock();

    // returns the current calendar time encoded as a time_t object
    time_t time(time_t* arg);

    // computes difference between two calendar times in seconds
    float64 difftime(time_t time_end, time_t time_beg)
    {
      return static_cast<float64>(time_end - time_beg);
    }

    // modifies the timespec object to hold the current calendar time in the time base base
    // int32 timespec_get(timespec* /*ts*/, int32 /*base*/)
    //{
    //
    //}

    /// RSL Comment: Different from ISO C++ Standard at time of writing (17/Jul/2022)
    // due to it being unsafe, rsl doesn't define ctime.
    // developers should use rsl::strftime instead.
    // char8* ctime(const time_t* time);

    /// RSL Comment: Different from ISO C++ Standard at time of writing (17/Jul/2022)
    // due to it being unsafe, rsl doesn't define ctime.
    // developers should use rsl::strftime instead.
    // char8* asctime(const tm* time);

    /// RSL Comment: Different from ISO C++ Standard at time of writing (17/Jul/2022)
    // rsl uses count_t for the return type and the "count" param of this func instead of size_t
    // converts the date and time information from a given calendar time time
    // to a null terminated multibyte character string according to format string format.
    count_t strftime(char8* str, count_t count, const char8* format, const tm* time);

    /// RSL Comment: Different from ISO C++ Standard at time of writing (17/Jul/2022)
    // rsl uses count_t for the return type and the "count" param of this func instead of size_t
    // converts the date and time information from a given calendar time time
    // to a null terminated wide character character string according to format string format.
    count_t wcsftime(tchar* str, count_t count, const tchar* format, const tm* time);

    // converts give time since epoch as time_t value into calendar time, expressed in UTC.
    tm* gmtime(const time_t* time);

    // converts given time since epoch as time_t value into calendar time, expressed in local time.
    tm* localtime(const time_t* time);

    // converts local calendar time to a time since epoch as a time_t object.
    time_t mktime(tm* time);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <ctime>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_FUNC_ALIAS(time);
    REX_STD_FUNC_ALIAS(clock);
    REX_STD_FUNC_ALIAS(timespec_get);

    REX_STD_FUNC_ALIAS(strftime);
    REX_STD_FUNC_ALIAS(wcsftime);
    REX_STD_FUNC_ALIAS(gmtime);
    REX_STD_FUNC_ALIAS(localtime);
    REX_STD_FUNC_ALIAS(mktime);

  } // namespace v1
} // namespace rsl

#include "rex_std/disable_std_checking.h"
// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: chrono.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/chrono.h"

#include <Windows.h>

namespace rsl
{
  inline namespace v1
  {
    namespace chrono::internal
    {
      uint64 get_ticks()
      {
        /// [24/Jul/2022] RSL TODO: Wrap this in ifdef for each platform
        auto query_frequency = []()
        {
          LARGE_INTEGER frequency;
          QueryPerformanceFrequency(&frequency);
          return double(1000000000.0L / frequency.QuadPart); // NOLINT nanoseconds per tick
        };

        auto query_counter = []()
        {
          LARGE_INTEGER counter;
          QueryPerformanceCounter(&counter);
          return counter.QuadPart;
        };

        static auto s_frequency = query_frequency();
        return static_cast<uint64>(s_frequency * query_counter()); // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
      }
    } // namespace chrono::internal
  }   // namespace v1
} // namespace rsl
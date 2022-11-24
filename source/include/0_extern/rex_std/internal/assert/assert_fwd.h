// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: assert_fwd.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// Declaration of assert functions and macros are defined here as well.
// The implementation of these functions are defined in assert_impl.h.
// This is so we can use assert in every header that assert depends on,
// because we have a circular dependency on these headers which we can't get rid of.
//-----------------------------------------------------------------------------

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename... Args>
    bool rex_assert(bool cond, Args&&... args);

  } // namespace v1
} // namespace rsl

/// RSL Comment: Different from ISO C++ Standard at time of writing (15/Sep/2022)
// The standard only accepts a condition in the assert macro,
// Rex Standard Library however accepts condition as well a message.
#ifdef REX_ENABLE_ASSERTS
  // #define assert(cond, ...) rsl::assert(cond, __VA_ARGS__)

  #define REX_ASSERT_X(cond, ...) rsl::rex_assert(cond, __VA_ARGS__)
  #define REX_ASSERT(...)         rsl::rex_assert(false, __VA_ARGS__)
#else
  // #define assert(cond, ...) // NOLINT(readability-identifier-naming)

  #define REX_ASSERT_X(cond, ...)
  #define REX_ASSERT(...)
#endif

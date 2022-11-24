// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: always_false.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T>
      inline constexpr bool always_false = false;
    } // namespace internal

  } // namespace v1
} // namespace rsl
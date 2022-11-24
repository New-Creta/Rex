// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: allocator_arg_t.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/memory/allocator_arg
//
// An empty class type used to disambiguate the overloads of constructors and member functions
// of allocator-aware objects.
//-----------------------------------------------------------------------------

#pragma once

namespace rsl
{
  inline namespace v1
  {

    struct allocator_arg_t
    {
      explicit allocator_arg_t() = default;
    };

    inline constexpr allocator_arg_t allocator_arg = allocator_arg_t();

  } // namespace v1
} // namespace rsl

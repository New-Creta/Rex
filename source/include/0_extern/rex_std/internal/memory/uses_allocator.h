// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: uses_allocator.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/memory/uses_allocator
//
// Checks if T has type alias of name allocator_type.
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/internal/type_traits/is_convertible.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T, typename Alloc, typename = void>
      struct has_allocator_type : false_type
      {
      }; // tests for suitable T::allocator_type

      template <typename T, typename Alloc>
      struct has_allocator_type<T, Alloc, void_t<typename T::allocator_type>> : is_convertible<Alloc, typename T::allocator_type>::type
      {
      }; // tests for suitable T::allocator_type

    } // namespace internal

    template <class T, class Alloc>
    struct uses_allocator : internal::has_allocator_type<T, Alloc>::type
    {
    };

    template <class T, class Alloc>
    inline constexpr bool uses_allocator_v = uses_allocator<T, Alloc>::value;

  } // namespace v1
} // namespace rsl

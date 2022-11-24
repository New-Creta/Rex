// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: typed_allocator.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/type_traits/is_const.h"
#include "rex_std/internal/utility/forward.h"
#include "rex_std/limits.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    class typed_allocator
    {
    public:
      static_assert(!rsl::is_const_v<T>, "The C++ standard forbids containers of const elements because typed_allocator<const T> is ill-formed");

      using value_type      = T;
      using size_type       = count_t;
      using difference_type = ptrdiff;

      /// RSL Comment: Different from ISO C++ Standard at time of writing (24/Jul/2022)
      // RSL doesn't support moving or copying if allocators differ,
      // so the following member types don't make much sense in having
      // using propagate_on_container_move_assignment = rsl::true_type;
      // using is_always_equal = rsl::true_type;

      // constructs the default allocator.
      typed_allocator() = default;
      // constructs the default allocator
      typed_allocator(const typed_allocator&) = default;
      // constructs the default allocator
      template <typename U>
      constexpr typed_allocator(const typed_allocator<U>& /*unused*/) // NOLINT(google-explicit-constructor)
      {
      }

      ~typed_allocator() = default;

      typed_allocator& operator=(const typed_allocator&) = default;

      // allocates n * sizeof(T) bytes of uninitialized storage
      REX_NO_DISCARD T* allocate(const size_type count)
      {
        return static_cast<T*>(operator new(sizeof(T) * count));
      }
      // deallocates the storage reference by the pointer p.
      // the pointer must be obtained by an earlier call to allocate
      // performed by this allocator or an allocator that's equal to this.
      void deallocate(T* const ptr, const size_type count)
      {
        operator delete(ptr, sizeof(T) * count);
      }

      // returns the maximum theoretically possible value of n for,
      // for which all calls to allocate(n) cold succeed.
      size_type max_size() const
      {
        return (rsl::numeric_limits<size_type>::max()) / sizeof(value_type);
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (24/Jul/2022)
      // the standard deprecates this in C++20 and moves it to std::allocator_traits
      // however, the definition of the word "traits" is the following
      // "a distinguishing quality or characteristic, typically one belonging to a person."
      // traits examples of an allocator would be size_type, pointer, reference, ...
      // we don't think member functions qualify as a trait of an object.
      // therefore we define construct() and destroy() members of the allocator class.

      // construct an object of type T in allocated uninitialized storage pointer to by p
      template <typename U, typename... Args>
      void construct(U* p, Args&&... args)
      {
        new(static_cast<void*>(p)) U(rsl::forward<Args>(args)...);
      }
      // calls the destructor of the object pointed to by p
      template <typename U>
      void destroy(U* p)
      {
        p->~U();
      }
    };

    // compares 2 default allocators.
    // default allocators are stateless, so they're always equal
    template <typename T1, typename T2>
    constexpr bool operator==(const typed_allocator<T1>& /*unused*/, const typed_allocator<T2>& /*unused*/)
    {
      return true;
    }
    // compares 2 default allocators.
    // default allocators are stateless, so they're always equal
    template <typename T1, typename T2>
    constexpr bool operator!=(const typed_allocator<T1>& /*unused*/, const typed_allocator<T2>& /*unused*/)
    {
      return false;
    }

  } // namespace v1
} // namespace rsl

// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: allocator.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/utility/forward.h"
#include "rex_std/limits.h"

#ifdef REX_ALLOCATOR_IS_TYPE_BASED
  #include "rex_std/bonus/memory/typed_allocator.h"
#endif

namespace rsl
{
  inline namespace v1
  {

// the only reason we have this define/config is to easily replace std calls with rsl calls
// when using std, use the type based allocator
#ifdef REX_ALLOCATOR_IS_TYPE_BASED
    template <typename T>
    using allocator = typed_allocator<T>;
#else
    /// RSL Comment: Different from ISO C++ Standard at time of writing (05/Aug/2022)
    // rex uses a non type based allocator by default.
    // this allocator allocates in number of bytes, not number of elements.
    class allocator
    {
    public:
      using size_type       = size_t;
      using difference_type = ptrdiff;

      /// RSL Comment: Different from ISO C++ Standard at time of writing (24/Jul/2022)
      // RSL doesn't support moving or copying if allocators differ,
      // so the following member types don't make much sense in having
      // using propagate_on_container_move_assignment = rsl::true_type;
      // using is_always_equal = rsl::true_type;

      // constructs the default allocator.
      allocator() = default;
      // constructs the default allocator
      allocator(const allocator&) = default;
      // constructs the default allocator
      allocator(allocator&&) = default;

      ~allocator() = default;

      allocator& operator=(const allocator&) = default;
      allocator& operator=(allocator&&)      = default;

      /// RSL Comment: Different from ISO C++ Standard at time of writing (23/Aug/2022)
      // because rex's allocator is not templated.
      // the following function is not defined
      // template <typename U>
      // constexpr allocator(const allocator<U> &);
      // allocates count bytes of uninitialized storage
      REX_NO_DISCARD void* allocate(const size_type count) // NOLINT(readability-convert-member-functions-to-static)
      {
        return (operator new(count));
      }
      // deallocates the storage reference by the pointer p.
      // the pointer must be obtained by an earlier call to allocate
      // performed by this allocator or an allocator that's equal to this.
      void deallocate(void* const ptr) // NOLINT(readability-convert-member-functions-to-static)
      {
        operator delete(ptr);
      }

      // returns the maximum theoretically possible value of n for,
      // for which all calls to allocate(n) could succeed.
      size_type max_size() const // NOLINT(readability-convert-member-functions-to-static)
      {
        return rsl::numeric_limits<size_type>::max();
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

    /// RSL Comment: Different from ISO C++ Standard at time of writing (23/Aug/2022)
    // because rex's allocator is not templated,
    // these functions differ from the standard's
    // compares 2 default allocators.
    // default allocators are stateless, so they're always equal
    constexpr bool operator==(const allocator& /*unused*/, const allocator& /*unused*/)
    {
      return true;
    }
    // compares 2 default allocators.
    // default allocators are stateless, so they're always equal
    constexpr bool operator!=(const allocator& /*unused*/, const allocator& /*unused*/)
    {
      return false;
    }
#endif

  } // namespace v1
} // namespace rsl

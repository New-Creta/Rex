#pragma once

#include "rex_engine/engine/types.h"

namespace rsl::test
{
  // An allocator to verify if the expected amount of allocations, deallocations and bytes allocated is followed

  class test_allocator
  {
  public:
    using size_type = size_t;
    using difference_type = ptrdiff;

    test_allocator() = default;
    test_allocator(const test_allocator&);
    test_allocator(test_allocator&&);

    ~test_allocator() = default;

    test_allocator& operator=(const test_allocator&) = default;
    test_allocator& operator=(test_allocator&&);

    RSL_NO_DISCARD void* allocate(const size_type count);
    void deallocate(void* const ptr, size_type size);

    size_type max_size() const;

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

    s32 num_allocs() const;
    s32 num_frees() const;
    s32 num_bytes_allocated() const;

    static s32 all_num_allocs();
    static s32 all_num_frees();
    static s32 all_num_bytes_allocated();
    static void all_reset();

    void reset();

  private:
    s32 m_num_allocs;
    s32 m_num_frees;
    s32 m_num_bytes_allocated;

    static s32 s_all_num_allocs;
    static s32 s_all_num_frees;
    static s32 s_all_num_bytes_allocated;
  };

  constexpr bool operator==(const test_allocator& /*unused*/, const test_allocator& /*unused*/)
  {
    return true;
  }
  constexpr bool operator!=(const test_allocator& /*unused*/, const test_allocator& /*unused*/)
  {
    return false;
  }
}
#pragma once

#include "rex_engine/memory/allocators/untracked_allocator.h"

#include "rex_std/memory.h"

namespace rex
{
  template <typename Allocator>
  class DebugAllocator
  {
  public:
    using size_type = typename rsl::allocator_traits<Allocator>::size_type;
    using pointer = typename rsl::allocator_traits<Allocator>::pointer;

    DebugAllocator()
      : m_allocator(nullptr)
    {
    }

    explicit DebugAllocator(Allocator& allocator)
      : m_allocator(rsl::addressof(allocator))
    {
    }

    REX_NO_DISCARD pointer allocate(size_type size)
    {
#ifndef REX_DISABLE_DEBUG_ALLOCATION
      return m_allocator->allocate(size);
#else
      REX_UNUSED_PARAM(size);
      return nullptr;
#endif
    }
    // We return a void* on purpose here as the following allocation
    // doesn't perform any initialization.
    // It just allocates the memory, the user is expected to use placement new
    // to initialize the memory for the type
    template <typename T>
    REX_NO_DISCARD T* allocate()
    {
      return static_cast<T*>(allocate(sizeof(T)));
    }
    void deallocate(pointer ptr, size_type size = 0)
    {
      m_allocator->deallocate(ptr, size);
    }
    template <typename T>
    void deallocate(T* ptr)
    {
      deallocate(ptr, sizeof(T));
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
      m_allocator->construct(p, rsl::forward<Args>(args)...);
    }
    template <typename T>
    void destroy(T* ptr)
    {
      m_allocator->destroy(ptr);
    }

    bool operator==(const DebugAllocator& rhs) const
    {
      return *m_allocator == *rhs.m_allocator;
    }
    bool operator!=(const DebugAllocator& rhs) const
    {
      return !(*this == rhs);
    }

  private:
    Allocator* m_allocator;
  };

  template <typename Alloc>
  DebugAllocator(Alloc) -> DebugAllocator<Alloc>;
}
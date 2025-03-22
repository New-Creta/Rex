#pragma once

#include "rex_engine/memory/allocators/tracked_allocator.h"
#include "rex_engine/memory/allocators/untracked_allocator.h"

#include "rex_std/memory.h"

namespace rex
{
  // The global allocator is the allocator that speaks directly to the OS to request memory
  // Allocations from the global allocator are to be avoided as they may require a kernell call
  // Prefer other global allocators of the engine instead

  // Type aliases
#ifdef REX_ENABLE_MEM_TRACKING
  using BackendOSAllocator = TrackedAllocator<UntrackedAllocator>;
#else
  using BackendOSAllocator = UntrackedAllocator;
#endif

  class GlobalAllocator
  {
  public:
    using size_type = card64;
    using pointer = void*;

    GlobalAllocator();

    REX_NO_DISCARD pointer allocate(rsl::memory_size size);
    REX_NO_DISCARD pointer allocate(size_type size);
    template <typename T>
    REX_NO_DISCARD T* allocate()
    {
      return static_cast<T*>(allocate(sizeof(T)));
    }

    void deallocate(pointer ptr, rsl::memory_size size = 0);
    void deallocate(pointer ptr, size_type size = 0);
    template <typename T>
    void deallocate(T* ptr)
    {
      return deallocate(ptr, sizeof(T));
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
      new(static_cast<void*>(p)) U(rsl::forward<Args>(args)...);
    }
    template <typename T>
    void destroy(T* ptr)
    {
      ptr->~T();
    }

    bool operator==(const UntrackedAllocator& /*other*/) const
    {
      return true;
    }
    bool operator!=(const UntrackedAllocator& rhs) const
    {
      return !(*this == rhs);
    }
  };
} // namespace rex
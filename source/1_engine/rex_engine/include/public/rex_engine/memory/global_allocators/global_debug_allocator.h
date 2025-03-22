#pragma once

#include "rex_engine/engine/types.h"

#include "rex_engine/memory/allocators/debug_allocator.h"
#include "rex_engine/memory/allocators/untracked_allocator.h"

#ifdef REX_BUILD_RELEASE
#define REX_DISABLE_DEBUG_ALLOCATION
#endif

namespace rex
{
  class GlobalDebugAllocator
  {
  public:
    void* allocate(const s32 count);    // deallocates the storage reference by the pointer p.
    void deallocate(void* const ptr, s32 count);
    template <typename T>
    T* allocate()
    {
      return static_cast<T*>(allocate(sizeof(T)));
    }

    s32 max_size() const;

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
      new(static_cast<void*>(p)) U(rsl::forward<Args>(args)...);
    }
    template <typename U>
    void destroy(U* p)
    {
      p->~U();
    }

  private:
    DebugAllocator<UntrackedAllocator> m_alloc;
  };

  constexpr bool operator==(const GlobalDebugAllocator& /*unused*/, const GlobalDebugAllocator& /*unused*/)
  {
    return true;
  }
  constexpr bool operator!=(const GlobalDebugAllocator& /*unused*/, const GlobalDebugAllocator& /*unused*/)
  {
    return false;
  }

} // namespace rex
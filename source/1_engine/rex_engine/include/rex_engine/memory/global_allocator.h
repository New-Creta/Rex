#pragma once

#include "rex_engine/memory/debug_allocator.h"
#include "rex_engine/memory/tracked_allocator.h"
#include "rex_engine/memory/untracked_allocator.h"

namespace rex
{
#ifdef REX_ENABLE_MEM_TRACKING
  using GlobalAllocator = TrackedAllocator<UntrackedAllocator>;
#else
  using GlobalAllocator = UntrackedAllocator;
#endif

  using GlobalDebugAllocator = DebugAllocator<UntrackedAllocator>;

  GlobalAllocator& global_allocator();
  GlobalDebugAllocator& global_debug_allocator();

  void* alloc(rsl::memory_size size);
  void dealloc(void* ptr, rsl::memory_size size);

  void* debug_alloc(rsl::memory_size size);
  void debug_dealloc(void* ptr, rsl::memory_size size);

  template <typename T, typename ... Args>
  T* alloc(Args&& ... args)
  {
    void* mem = alloc(sizeof(T));
    new (mem) T(rsl::forward<Args>(args)...);
    return static_cast<T*>(mem);
  }
  template <typename T, typename ... Args>
  T* debug_alloc(Args&& ... args)
  {
    void* mem = debug_alloc(sizeof(T));
    new (mem) T(rsl::forward<Args>(args)...);
    return static_cast<T*>(mem);
  }

  template <typename T, typename ... Args>
  rsl::unique_ptr<T> make_unique_debug(Args&& ... args)
  {
    rsl::unique_ptr<T> ptr(debug_alloc<T>(rsl::forward<Args>(args)...));
    return ptr;
  }

} // namespace rex
#pragma once

#include "rex_engine/memory/global_allocators/global_debug_allocator.h"
#include "rex_engine/memory/allocators/tracked_allocator.h"
#include "rex_engine/memory/allocators/untracked_allocator.h"

#include "rex_std/memory.h"

namespace rex
{
  // Type aliases
#ifdef REX_ENABLE_MEM_TRACKING
  using GlobalAllocator = TrackedAllocator<UntrackedAllocator>;
#else
  using GlobalAllocator = UntrackedAllocator;
#endif

  using GlobalDebugAllocator = DebugAllocator<UntrackedAllocator>;

  // Accessors
  GlobalAllocator& global_allocator();
  GlobalDebugAllocator& global_debug_allocator();

  // Global memory allocations
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
  template <typename T>
  void dealloc(T* ptr)
  {
    ptr->~T();
    dealloc(ptr, sizeof(T));
  }
  template <typename T, typename ... Args>
  T* debug_alloc(Args&& ... args)
  {
    void* mem = debug_alloc(sizeof(T));
    new (mem) T(rsl::forward<Args>(args)...);
    return static_cast<T*>(mem);
  }
  template <typename T>
  void debug_dealloc(T* ptr)
  {
    ptr->~T();
    debug_dealloc(ptr, sizeof(T));
  }

  // Debug Unique Pointer
  template <typename T>
  struct DebugDelete
  {
  public:
    constexpr DebugDelete() = default;

    template <typename T2, rsl::enable_if_t<rsl::is_convertible_v<T2*, T*>, bool> = true>
    constexpr DebugDelete(const DebugDelete<T2>& /*unused*/) // NOLINT(google-explicit-constructor)
    {
    }

    constexpr void operator()(T* ptr) const
    {
      static_assert(sizeof(T) > 0, "can't delete an incomplete type"); // NOLINT(bugprone-sizeof-expression)
      debug_dealloc(ptr);
    }
  };
  template <typename T>
  using unique_debug_ptr = rsl::unique_ptr<T, DebugDelete<T>>;

  template <typename T, typename ... Args>
  unique_debug_ptr<T> make_unique_debug(Args&& ... args)
  {
    unique_debug_ptr<T> ptr(debug_alloc<T>(rsl::forward<Args>(args)...));
    return ptr;
  }

} // namespace rex
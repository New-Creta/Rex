#pragma once

#include "rex_std/memory.h"
#include "rex_std/type_traits.h"
#include "rex_std/utility.h"

#include "rex_engine/memory/global_allocators/global_debug_allocator.h"
#include "rex_engine/memory/global_allocators/global_scratch_allocator.h"
#include "rex_engine/memory/global_allocators/global_single_frame_allocator.h"

namespace rex
{
  template <typename T, typename Allocator>
  class DeleterWithAllocator
  {
  public:
    template<typename, typename> friend class DeleterWithAllocator;

    DeleterWithAllocator()
      : m_alloc()
    {}

    DeleterWithAllocator(const Allocator& alloc)
      : m_alloc(alloc)
    {}

    template <typename T2, rsl::enable_if_t<rsl::is_convertible_v<T2*, T*>, bool> = true>
    DeleterWithAllocator(const DeleterWithAllocator<T2, Allocator>& other) // NOLINT(google-explicit-constructor)
    {
      m_alloc = other.m_alloc;
    }

    void operator()(T* ptr)
    {
      static_assert(sizeof(T) > 0, "can't delete an incomplete type"); // NOLINT(bugprone-sizeof-expression)
      m_alloc.deallocate(ptr, sizeof(T));
    }

    void operator()(T* ptr, card32 size)
    {
      static_assert(sizeof(T) > 0, "can't delete an incomplete type"); // NOLINT(bugprone-sizeof-expression)
      m_alloc.deallocate(ptr, size);
    }

  private:
    Allocator m_alloc;
  };

  //----------------------
  // Unique Ptr
  //----------------------
  template <typename T, typename Allocator, typename... Args, rsl::enable_if_t<!rsl::is_array_v<T>, bool> = true>
  RSL_NO_DISCARD rsl::unique_ptr<T, DeleterWithAllocator<T, Allocator>> alloc_unique(Args&&... args)
  {
    Allocator alloc;
    return alloc_unique<T>(alloc, rsl::forward<Args>(args)...);
  }

  template <typename T, typename Allocator, typename... Args, rsl::enable_if_t<!rsl::is_array_v<T>, bool> = true>
  RSL_NO_DISCARD rsl::unique_ptr<T, DeleterWithAllocator<T, Allocator>> alloc_unique(Allocator& alloc, Args&&... args)
  {
    T* ptr = alloc.allocate<T>();
    alloc.construct(ptr, rsl::forward<Args>(args)...);
    DeleterWithAllocator<T, Allocator> deleter(alloc);
    return rsl::unique_ptr<T, DeleterWithAllocator<T, Allocator>>(ptr, deleter);
  }

  //----------------------
  // Unique Array
  //----------------------
  template <typename T, typename Allocator, rsl::enable_if_t<rsl::is_array_v<T>, bool> = true>
  RSL_NO_DISCARD rsl::unique_array<rsl::remove_extent_t<T>, DeleterWithAllocator<rsl::remove_extent_t<T>, Allocator>> alloc_unique(card32 size)
  {
    Allocator alloc;
    return alloc_unique<T>(alloc, size);
  }

  template <typename T, typename Allocator, rsl::enable_if_t<rsl::is_array_v<T>, bool> = true>
  RSL_NO_DISCARD rsl::unique_array<rsl::remove_extent_t<T>, DeleterWithAllocator<rsl::remove_extent_t<T>, Allocator>> alloc_unique(Allocator& alloc, card32 size)
  {
    using Elem = rsl::remove_extent_t<T>;
    Elem* ptr = static_cast<Elem*>(alloc.allocate(sizeof(Elem) * size));
    DeleterWithAllocator<Elem, Allocator> deleter(alloc);
    return rsl::unique_array<rsl::remove_extent_t<T>, DeleterWithAllocator<rsl::remove_extent_t<T>, Allocator>>(ptr, size, deleter);
  }

  //----------------------
  // Debug Allocator
  //----------------------
  template <typename T, typename... Args, rsl::enable_if_t<!rsl::is_array_v<T>, bool> = true>
  RSL_NO_DISCARD rsl::unique_ptr<T, DeleterWithAllocator<T, GlobalDebugAllocator>> alloc_unique_debug(Args&&... args)
  {
    GlobalDebugAllocator alloc;
    return alloc_unique<T>(alloc, rsl::forward<Args>(args)...);
  }

  template <typename T, rsl::enable_if_t<rsl::is_array_v<T>, bool> = true>
  RSL_NO_DISCARD rsl::unique_array<rsl::remove_extent_t<T>, DeleterWithAllocator<rsl::remove_extent_t<T>, GlobalDebugAllocator>> alloc_unique_debug(card32 size)
  {
    GlobalDebugAllocator alloc;
    return alloc_unique<T>(alloc, size);
  }

  //----------------------
  // Scratch Allocator
  //----------------------
  template <typename T, typename... Args, rsl::enable_if_t<!rsl::is_array_v<T>, bool> = true>
  RSL_NO_DISCARD rsl::unique_ptr<T, DeleterWithAllocator<T, GlobalScratchAllocator>> alloc_unique_scratch(Args&&... args)
  {
    GlobalScratchAllocator alloc;
    return alloc_unique<T>(alloc, rsl::forward<Args>(args)...);
  }

  template <typename T, rsl::enable_if_t<rsl::is_array_v<T>, bool> = true>
  RSL_NO_DISCARD rsl::unique_array<rsl::remove_extent_t<T>, DeleterWithAllocator<rsl::remove_extent_t<T>, GlobalScratchAllocator>> alloc_unique_scratch(card32 size)
  {
    GlobalScratchAllocator alloc;
    return alloc_unique<T>(alloc, size);
  }

  //----------------------
  // Single Frame Allocator
  //----------------------
  template <typename T, typename... Args, rsl::enable_if_t<!rsl::is_array_v<T>, bool> = true>
  RSL_NO_DISCARD rsl::unique_ptr<T, DeleterWithAllocator<T, GlobalSingleFrameAllocator>> alloc_unique_temp(Args&&... args)
  {
    GlobalSingleFrameAllocator alloc;
    return alloc_unique<T>(alloc, rsl::forward<Args>(args)...);
  }

  template <typename T, rsl::enable_if_t<rsl::is_array_v<T>, bool> = true>
  RSL_NO_DISCARD rsl::unique_array<rsl::remove_extent_t<T>, DeleterWithAllocator<rsl::remove_extent_t<T>, GlobalSingleFrameAllocator>> alloc_unique_temp(card32 size)
  {
    GlobalSingleFrameAllocator alloc;
    return alloc_unique<T>(alloc, size);
  }
}
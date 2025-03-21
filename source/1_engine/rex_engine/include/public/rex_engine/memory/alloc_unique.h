#pragma once

#include "rex_std/memory.h"
#include "rex_std/type_traits.h"
#include "rex_std/utility.h"

namespace rex
{
  template <typename T, typename Allocator>
  class DeleterWithAllocator
  {
  public:
    DeleterWithAllocator(const Allocator& alloc)
      : m_alloc(alloc)
    {}

    template <typename T2, rsl::enable_if_t<rsl::is_convertible_v<T2*, T*>, bool> = true>
    DeleterWithAllocator(const DeleterWithAllocator<T2, Allocator>& /*unused*/) // NOLINT(google-explicit-constructor)
    {
      m_alloc = alloc;
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

  template <typename T, typename Allocator, typename... Args, rsl::enable_if_t<!rsl::is_array_v<T>, bool> = true>
  RSL_NO_DISCARD rsl::unique_ptr<T> alloc_unique(Allocator& alloc, Args&&... args)
  {
    T* ptr = alloc.allocate<T>();
    alloc.construct(ptr, rsl::forward<Args>(args));
    DeleterWithAllocator<T, Allocator> deleter(alloc);
    return rsl::unique_ptr<T, DeleterWithAllocator<T, Allocator>>(ptr, deleter);
  }

  template <typename T, typename Allocator, rsl::enable_if_t<rsl::is_array_v<T>, bool> = true>
  RSL_NO_DISCARD rsl::unique_array<rsl::remove_extent_t<T>, DeleterWithAllocator<rsl::remove_extent_t<T>, Allocator>> alloc_unique(Allocator& alloc, card32 size)
  {
    using Elem = rsl::remove_extent_t<T>;
    Elem* ptr = static_cast<Elem*>(alloc.allocate(sizeof(Elem) * size));
    DeleterWithAllocator<Elem, Allocator> deleter(alloc);
    return rsl::unique_array<rsl::remove_extent_t<T>, DeleterWithAllocator<rsl::remove_extent_t<T>, Allocator>>(ptr, size, deleter);
  }

}
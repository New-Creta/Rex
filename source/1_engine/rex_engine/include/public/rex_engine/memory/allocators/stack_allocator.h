#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/defines.h"

#include "rex_std/memory.h"

namespace rex
{
  using StackMarker = s32;

  class StackAllocator
  {
  public:
    using size_type = s32;
    using pointer = void*;

    explicit StackAllocator(rsl::unique_array<rsl::byte> buffer);

    // Allocate size bytes from the underlying buffer and return a pointer to it
    REX_NO_DISCARD pointer allocate(size_type size, size_type alignment);
    // Allocate size bytes from the underlying buffer and return a pointer to it
    RSL_NO_DISCARD pointer allocate(size_type size);
    template <typename T>
    REX_NO_DISCARD T* allocate()
    {
      return static_cast<T*>(allocate(sizeof(T), alignof(T)));
    }

    // This does nothing internally but is only provided to follow basic allocator interface
    void deallocate(pointer ptr, size_type size = 0);

    template <typename U, typename... Args>
    void  construct(U* p, Args&&... args)
    {
      new (p) (rsl::forward<Args>(args)...);
    }
    template <typename T>
    void destroy(T* ptr)
    {
      ptr->~T();
    }

    bool operator==(const StackAllocator& rhs) const;
    bool operator!=(const StackAllocator& rhs) const;

    // Reset the current stack pointer to the beginning
    // New allocations will starts from the fron again
    void reset();

    // Return the offset from the beginning of the buffer allocations will be made from
    StackMarker current_marker() const;
    // Set the offset within the buffer allocations should start from
    void set_marker(StackMarker marker);

    bool can_allocate(size_type size, size_type alignment = 1) const;
    size_type num_allocated() const;

  private:
    rsl::unique_array<rsl::byte> m_buffer;
    rsl::byte* m_current_marker;
  };
}
#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/defines.h"
#include "rex_engine/engine/casting.h"
#include "rex_engine/memory/alloc_unique.h"

#include "rex_std/memory.h"

namespace rex
{
  using StackMarker = s32;

  template <typename BackendAllocator>
  class StackAllocator
  {
  public:
    using size_type = s32;
    using pointer = void*;

    StackAllocator()
      : m_buffer(nullptr)
      , m_current_marker(nullptr)
    {}
    explicit StackAllocator(size_type size, BackendAllocator alloc = BackendAllocator())
    {
      m_buffer = alloc_unique<rsl::byte[]>(alloc, size);
      m_current_marker = m_buffer.get();
    }

    // Allocate size bytes from the underlying buffer and return a pointer to it
    REX_NO_DISCARD pointer allocate(size_type size, size_type alignment)
    {
      RSL_ASSERT_X(can_allocate(size, alignment), "Stack allocator out of bounds! max size: ", m_buffer.count(), " new size: ", current_marker() + size);

      m_current_marker = align(m_current_marker, alignment);
      void* mem = m_current_marker;
      m_current_marker += size;
      return mem;
    }

    // Allocate size bytes from the underlying buffer and return a pointer to it
    RSL_NO_DISCARD pointer allocate(size_type size)
    {
      s32 alignment = static_cast<s32>(alignof(rsl::max_align));
      return allocate(size, alignment);
    }

    template <typename T>
    REX_NO_DISCARD T* allocate()
    {
      return static_cast<T*>(allocate(sizeof(T), alignof(T)));
    }

    // This does nothing internally but is only provided to follow basic allocator interface
    void deallocate(pointer ptr, size_type size = 0)
    {
      // Nothing to implement
      REX_UNUSED_PARAM(ptr);
      REX_UNUSED_PARAM(size);
    }

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

    bool operator==(const StackAllocator& rhs) const
    {
      return m_buffer.get() == rhs.m_buffer.get();
    }

    bool operator!=(const StackAllocator& rhs) const
    {
      return !(*this == rhs);
    }

    // Reset the current stack pointer to the beginning
    // New allocations will starts from the fron again
    void reset()
    {
      set_marker(0);
    }

    // Return the offset from the beginning of the buffer allocations will be made from
    StackMarker current_marker() const
    {
      return num_allocated();
    }

    // Set the offset within the buffer allocations should start from
    void set_marker(StackMarker marker)
    {
      m_current_marker = &m_buffer[marker];
    }

    bool can_allocate(size_type size, size_type alignment = 1) const
    {
      return m_current_marker + size + alignment <= m_buffer.get() + m_buffer.count();
    }

    size_type num_allocated() const
    {
      return narrow_cast<size_type>(m_current_marker - m_buffer.get());
    }

  private:
    rsl::unique_array<rsl::byte, DeleterWithAllocator<rsl::byte, BackendAllocator>> m_buffer;
    rsl::byte* m_current_marker;
  };
}
#include "rex_engine/memory/allocators/stack_allocator.h"

#include "rex_engine/memory/pointer_math.h"

namespace rex
{
  StackAllocator::StackAllocator(rsl::unique_array<rsl::byte> buffer)
    : m_buffer(rsl::move(buffer)) // NOLINT(modernize-avoid-c-arrays)
    , m_current_marker(m_buffer.get())
  {
  }

  StackAllocator::pointer StackAllocator::allocate(const size_type size, size_type alignment)
  {
    RSL_ASSERT_X(can_allocate(size, alignment), "Stack allocator out of bounds! max size: ", m_buffer.count(), " new size: ", current_marker() + size);

    m_current_marker = align(m_current_marker, alignment);
    void* mem = m_current_marker;
    m_current_marker += size;
    return mem;
  }
  StackAllocator::pointer StackAllocator::allocate(size_type size)
  {
    u64 alignment = alignof(rsl::max_align);
    return allocate(size, alignment);
  }
  void StackAllocator::deallocate(pointer /*ptr*/, const size_type /*size*/)
  {
    // Nothing to implement
  }

  bool StackAllocator::operator==(const StackAllocator& rhs) const
  {
    return m_buffer.get() == rhs.m_buffer.get();
  }
  bool StackAllocator::operator!=(const StackAllocator & rhs) const
  {
    return !(*this == rhs);
  }

  void StackAllocator::reset()
  {
    set_marker(0);
  }

  StackMarker StackAllocator::current_marker() const
  {
    return num_allocated();
  }

  void StackAllocator::set_marker(StackMarker marker)
  {
    m_current_marker = &m_buffer[marker];
  }

  bool StackAllocator::can_allocate(size_type size, size_type alignment) const
  {
    return m_current_marker + size + alignment <= m_buffer.get() + m_buffer.count();
  }

  StackAllocator::size_type StackAllocator::num_allocated() const
  {
    return m_current_marker - m_buffer.get();
  }

}
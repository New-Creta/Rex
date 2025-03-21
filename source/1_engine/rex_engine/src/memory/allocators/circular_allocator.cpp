#include "rex_engine/memory/allocators/circular_allocator.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/memory/pointer_math.h"

#include "rex_engine/engine/casting.h"

namespace rex
{
	CircularAllocator::CircularAllocator(rsl::unique_array<rsl::byte> buffer)
		: m_buffer(rsl::move(buffer))
		, m_current(m_buffer.get())
		, m_end(m_buffer.get() + m_buffer.count())
	{

	}
	CircularAllocator::pointer CircularAllocator::allocate(size_type size, size_type alignment)
	{
		REX_ASSERT_X(size <= m_buffer.count(), "Allocating more data the circular buffer holds.");

		// As we're not given a type, we need to make sure our memory is aligned within the buffer
		m_current = align(m_current, alignment);

		s32 space_available_until_end = narrow_cast<s32>(m_end - m_current);
		if (space_available_until_end < size)
		{
			// If we can't find the entire allocation
			// We need to reset the allocation pointer to the front
			// this will introduce some wasted memory usage
			// but is better for performance as data will be aligned
			// and it's more cache friendly as well
			m_current = m_buffer.get();
			m_current = align(m_current, alignment);
		}

		m_current += size;

		return m_current;
	}

	CircularAllocator::pointer CircularAllocator::allocate(size_type size)
	{
		s32 alignment = narrow_cast<s32>(alignof(rsl::max_align));
		return allocate(size, alignment);
	}
	void CircularAllocator::deallocate(pointer ptr, size_type size)
	{
		// Nothing to implement

		REX_UNUSED_PARAM(ptr);
		REX_UNUSED_PARAM(size);
	}

	bool CircularAllocator::operator==(const CircularAllocator& rhs) const
	{
		return m_buffer.get() == rhs.m_buffer.get();
	}
	bool CircularAllocator::operator!=(const CircularAllocator& rhs) const
	{
		return !(*this == rhs);
	}
}
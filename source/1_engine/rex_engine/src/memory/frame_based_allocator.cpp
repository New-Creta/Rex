#include "rex_engine/memory/frame_based_allocator.h"

#include "rex_engine/diagnostics/assert.h"

#include "rex_engine/engine/globals.h"

namespace rex
{
	FrameBasedAllocator::FrameBasedAllocator(rsl::memory_size size, s32 maxFrames)
		: m_stack_allocator(static_cast<count_t>(size))
		, m_max_frames(maxFrames)
		, m_current_frame(0)
	{
		REX_ASSERT_X(m_max_frames > 0, "The max frame value of a frame based allocator should be larger than 0");
	}

	void* FrameBasedAllocator::allocate(s32 size)
	{
		return m_stack_allocator.allocate(size);
	}
	void FrameBasedAllocator::deallocate(void* ptr)
	{
		m_stack_allocator.deallocate(ptr, 0);
	}

	void FrameBasedAllocator::set_marker(s32 offset)
	{
		m_stack_allocator.set_marker(offset);
	}
	s32 FrameBasedAllocator::get_marker()
	{
		return m_stack_allocator.current_marker();
	}
	void FrameBasedAllocator::adv_frame()
	{
		++m_current_frame;
		if (m_current_frame == m_max_frames)
		{
			m_stack_allocator.reset();
			m_current_frame = 0;
		}
	}

	s32 FrameBasedAllocator::max_frames() const
	{
		return m_max_frames;
	}

	TempHeapScope::TempHeapScope()
	{
		m_is_released = false;
		m_marker = get_marker();
	}
	TempHeapScope::~TempHeapScope()
	{
		if (!m_is_released)
		{
			set_current_marker(m_marker);
		}
	}

	void TempHeapScope::reset_to(s32 offsetFromMarker)
	{
		set_current_marker(m_marker + offsetFromMarker);
		release();
	}
	void TempHeapScope::release()
	{
		m_is_released = true;
	}

}
#include "rex_engine/memory/frame_based_allocator.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
	FrameBasedAllocator::FrameBasedAllocator(rsl::memory_size size, s32 maxFrames)
		: m_stack_allocator(size)
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

}
#pragma once

#include "rex_engine/engine/types.h"

#include "rex_std/bonus/memory/stack_allocator.h"
#include "rex_std/bonus/memory/memory_size.h"

namespace rex
{
	class FrameBasedAllocator
	{
	public:
		FrameBasedAllocator(rsl::memory_size size, s32 maxFrames);

		void* allocate(s32 size);
		void deallocate(void* ptr);

		// Advance the allocator 1 frame
		// If the max frame number is reached, the internal allocator gets reset
		void adv_frame();

	private:
		// The number of frames it keeps memory active
		s32 m_max_frames;
		s32 m_current_frame;
		rsl::stack_allocator m_stack_allocator;
	};
}
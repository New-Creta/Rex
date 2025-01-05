#include "rex_engine/engine/globals.h"
#include "rex_engine/engine/mutable_globals.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
	Globals g_globals{};

	const Globals& globals()
	{
		return g_globals;
	}

	Globals& mut_globals()
	{
		return g_globals;
	}

	void set_single_frame_alloc(rsl::unique_ptr<FrameBasedAllocator> singleFrameAlloc)
	{
		REX_ASSERT_X(singleFrameAlloc->max_frames() == 1, "A single frame allocator may only have 1 frame active");
		g_globals.single_frame_allocator = rsl::move(singleFrameAlloc);
	}

	void set_current_marker(s32 offset)
	{
		g_globals.single_frame_allocator->set_marker(offset);
	}
	s32 get_marker()
	{
		return g_globals.single_frame_allocator->get_marker();
	}


}

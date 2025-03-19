#pragma once

#include "rex_engine/memory/frame_based_allocator.h"
#include "rex_engine/frameinfo/frameinfo.h"

namespace rex
{
	// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: REX GLOBALS

	struct Globals
	{
		rsl::unique_ptr<FrameBasedAllocator> single_frame_allocator;

		FrameInfo frame_info;
	};

	// Globals are tied together so we have a strict initialization order
	// Having this also avoids the discussion how globals are accessed
	// - No singletons
	// - No local statics
	// - No pure global variables
	// All engine globals are accessed through the below func
	const Globals& globals();

	void set_single_frame_alloc(rsl::unique_ptr<FrameBasedAllocator> singleFrameAlloc);
	void set_current_marker(s32 offset);
	s32 get_marker();
}
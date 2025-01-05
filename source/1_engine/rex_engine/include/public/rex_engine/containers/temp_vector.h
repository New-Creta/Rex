#pragma once

#include "rex_std/vector.h"

#include "rex_engine/memory/single_frame_allocator_wrapper.h"

namespace rex
{
	template <typename T>
	using TempVector = rsl::vector<T, SingleFrameAllocatorWrapper>;
}
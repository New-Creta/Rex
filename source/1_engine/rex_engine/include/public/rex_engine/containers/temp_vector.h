#pragma once

#include "rex_std/vector.h"

#include "rex_engine/memory/single_frame_allocator_wrapper.h"

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: HEAP AND ALLOCATORS
namespace rex
{
	template <typename T>
	using TempVector = rsl::vector<T, SingleFrameAllocatorWrapper>;
}
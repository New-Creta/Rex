#include "rex_engine/memory/memory_query.h"

#include "rex_engine/memory/global_allocators/global_single_frame_allocator.h"
#include "rex_engine/memory/global_allocators/global_scratch_allocator.h"

namespace rex
{
	bool is_global_memory(void* ptr)
	{
		return !is_temp_memory(ptr) && !is_scratch_memory(ptr);
	}
	bool is_temp_memory(void* ptr)
	{
		return GlobalSingleFrameAllocator().has_allocated_ptr(ptr);
	}
	bool is_scratch_memory(void* ptr)
	{
		return GlobalScratchAllocator().has_allocated_ptr(ptr);
	}
}
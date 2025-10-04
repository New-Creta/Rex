#include "rex_engine/memory/memory.h"

namespace rex
{
	// Allocations that last until they get deallocated
	void* alloc(u64 size)
	{
		return GlobalAllocator().allocate(size);
	}
	void* realloc(void* p, u64 size)
	{
		return GlobalAllocator().reallocate(p, size);
	}
	void dealloc(void* p, u64 size)
	{
		GlobalAllocator().deallocate(p, size);
	}

	// Allocations that last until they get deallocatoed, but only in debug builds
	void* debug_alloc(u64 size)
	{
		return GlobalDebugAllocator().allocate(size);
	}
	void* debug_realloc(void* p, u64 size)
	{
		return GlobalDebugAllocator().reallocate(p, size);
	}
	void debug_dealloc(void* p, u64 size)
	{
		GlobalDebugAllocator().deallocate(p, size);
	}

	// Allocations who's lifetime isn't well determined
	// They get allocated from an internal ring buffer
	void* scratch_alloc(u64 size)
	{
		return GlobalScratchAllocator().allocate(size);
	}
	void* scratch_realloc(void* p, u64 size)
	{
		return GlobalScratchAllocator().reallocate(p, size);
	}
	void scratch_dealloc(void* p, u64 size)
	{
		GlobalScratchAllocator().deallocate(p, size);
	}

	// allocations that only last a single frame
	void* temp_alloc(u64 size)
	{
		return GlobalSingleFrameAllocator().allocate(narrow_cast<s32>(size));
	}
	void* temp_realloc(void* p, u64 size)
	{
		return GlobalSingleFrameAllocator().reallocate(p, size);
	}
	void temp_dealloc(void* p, u64 size)
	{
		return GlobalSingleFrameAllocator().deallocate(p, narrow_cast<s32>(size));
	}

}
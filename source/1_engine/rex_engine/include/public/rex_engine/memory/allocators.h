#pragma once

#include "rex_std/bonus/memory.h"

#include "rex_engine/memory/ring_buffer.h"
#include "rex_engine/memory/typeless_ring_buffer.h"

namespace rex
{
	// --------------------------
	// Heaps
	// --------------------------

	class IHeap
	{
	public:
		virtual void* allocate();
		virtual void deallocate();

		virtual void clear();

		virtual bool can_alloc();

		virtual rsl::memory_size total_memory() const;
		virtual rsl::memory_size allocated_memory() const;
		virtual rsl::memory_size available_memory() const;
	private:

	};

	// 1. circular heap
	
	// 2. free list heap
	
	// 3. stack based heap
	// 4. double buffered heap
	// 5. tagged heap
	// 6. linear heap

	// --------------------------
	// Allocators
	// --------------------------



	// --------------------------
	// Types
	// --------------------------

}
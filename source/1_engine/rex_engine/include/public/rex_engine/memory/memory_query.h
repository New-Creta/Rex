#pragma once

namespace rex
{
	// Returns true if the memory is global.
	// It does not differentiate between debug and non-debug memory
	bool is_global_memory(void* ptr);

	// Returns true if the memory is only temporary
	// eg. it being part of the single frame allocator
	bool is_temp_memory(void* ptr);

	// Returns true if the memory is scratch memory
	// eg. it being allocated from the scratch memory allocator
	bool is_scratch_memory(void* ptr);
}
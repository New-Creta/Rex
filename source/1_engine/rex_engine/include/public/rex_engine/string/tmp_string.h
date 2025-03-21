#pragma once

#include "rex_engine/engine/types.h"

#include "rex_std/string.h"
#include "rex_std/string_view.h"

//#include "rex_engine/memory/allocators/single_frame_allocator_wrapper.h"

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: HEAP AND ALLOCATORS

namespace rex
{
	// This string should not be accessed directly
	// It's memory is not owned by this class but owned by something else instead (often the single frame allocator)
	// If a user wants to operate on it during the same frame it's allocated (perhaps in the same function scope)
	// the user should convert it to a string_view.
	// If the storage of the tmp string is expected to last for longer than a single frame, it should be converted
	// to a string instead.
	using TempString = rsl::string;
	template <typename T>
	using TempVector = rsl::vector<T>;
}
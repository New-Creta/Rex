#pragma once

#include "rex_engine/engine/types.h"

#include "rex_engine/memory/global_allocators/global_debug_allocator.h"
#include "rex_engine/memory/global_allocators/global_scratch_allocator.h"
#include "rex_engine/memory/global_allocators/global_single_frame_allocator.h"

#include "rex_engine/memory/alloc_unique.h"

#include "rex_std/vector.h"
#include "rex_std/memory.h"
#include "rex_std/map.h"
#include "rex_std/unordered_map.h"
#include "rex_std/string.h"

namespace rex
{
	//----------------------
	// Debug Allocator
	//----------------------
	template <typename T>
	using debug_unique_ptr			= rsl::unique_ptr<T, DeleterWithAllocator<T, GlobalDebugAllocator>>;
	template <typename T>
	using debug_vector					= rsl::vector<T, GlobalDebugAllocator>;
	template <typename Key, typename Value>
	using debug_hash_map				= rsl::unordered_map<Key, Value, rsl::hash<Key>, rsl::equal_to<Key>, GlobalDebugAllocator>;
	template <typename Key, typename Value>
	using debug_map							= rsl::map<Key, Value, rsl::less<Key>, GlobalDebugAllocator>;
	using debug_string					= rsl::basic_string<char8, rsl::char_traits<char8>, GlobalDebugAllocator>;
	using debug_string_stream   = rsl::basic_stringstream<char8, rsl::char_traits<char8>, GlobalDebugAllocator>;


	//----------------------
	// Scratch Allocator
	//----------------------
	template <typename T>
	using scratch_unique_ptr		= rsl::unique_ptr<T, DeleterWithAllocator<T, GlobalScratchAllocator>>;
	template <typename T>
	using scratch_vector				= rsl::vector<T, GlobalScratchAllocator>;
	template <typename Key, typename Value>
	using scratch_hash_map			= rsl::unordered_map<Key, Value, rsl::hash<Key>, rsl::equal_to<Key>, GlobalScratchAllocator>;
	template <typename Key, typename Value>
	using scratch_map						= rsl::map<Key, Value, rsl::less<Key>, GlobalScratchAllocator>;
	using scratch_string				= rsl::basic_string<char8, rsl::char_traits<char8>, GlobalScratchAllocator>;
	using scratch_string_stream = rsl::basic_stringstream<char8, rsl::char_traits<char8>, GlobalScratchAllocator>;

	//----------------------
	// Single Frame Allocator
	//----------------------
	template <typename T>
	using temp_unique_ptr				= rsl::unique_ptr<T, DeleterWithAllocator<T, GlobalSingleFrameAllocator>>;
	template <typename T>
	using temp_vector						= rsl::vector<T, GlobalSingleFrameAllocator>;
	template <typename Key, typename Value>
	using temp_hash_map					= rsl::unordered_map<Key, Value, rsl::hash<Key>, rsl::equal_to<Key>, GlobalSingleFrameAllocator>;
	template <typename Key, typename Value>
	using temp_map							= rsl::map<Key, Value, rsl::less<Key>, GlobalSingleFrameAllocator>;
	using temp_string						= rsl::basic_string<char8, rsl::char_traits<char8>, GlobalSingleFrameAllocator>;
	using temp_string_stream = rsl::basic_stringstream<char8, rsl::char_traits<char8>, GlobalSingleFrameAllocator>;

}
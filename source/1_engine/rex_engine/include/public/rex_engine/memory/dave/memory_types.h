#pragma once

#include "memory/allocators/generic_heap_allocator.h"
#include "memory/allocators/tagged_heap_allocator.h"

#include "memory/allocators/heap_allocator_deleter.h"
#include "memory/allocators/heap_allocator_policy.h"

#include "memory/memory_tags.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <string>
#include <map>

namespace ppp
{
    //-----------------------------------------------------------------------------
    // 
    // PERSISTANT ALLOCATORS
    // 
    //-----------------------------------------------------------------------------

    namespace memory
    {
        // Allocator using the persistent tagged heap.
        template<typename T>
        using persistent_global_allocator = memory::generic_heap_allocator<T, persistent_global_policy>;
        template<typename T>
        using persistent_frame_allocator = memory::generic_heap_allocator<T, persistent_frame_policy>;
        template<typename T>
        using persistent_scratch_allocator = memory::generic_heap_allocator<T, persistent_scratch_policy>;

        // Type aliases for persistent allocators.
        template<typename T>
        using persistent_graphics_tagged_allocator = memory::tagged_heap_allocator<T, persistent_tagged_policy, memory::tags::graphics>;
        template<typename T>
        using persistent_fileio_tagged_allocator = memory::tagged_heap_allocator<T, persistent_tagged_policy, memory::tags::fileio>;
        template<typename T>
        using persistent_global_tagged_allocator = memory::tagged_heap_allocator<T, persistent_tagged_policy, memory::tags::global>;
        template<typename T>
        using persistent_stage_tagged_allocator = memory::tagged_heap_allocator<T, persistent_tagged_policy, memory::tags::stage>;
    }

    // Unique Ptrs 
    // Type alias for a unique_ptr that uses the 'global' tag
    template <typename T>
    using global_unique_ptr = std::unique_ptr<T, memory::allocator_deleter<T, memory::persistent_global_tagged_allocator<T>>>;
    // Type alias for a unique_ptr that uses the 'graphics' tag
    template <typename T>
    using graphics_unique_ptr = std::unique_ptr<T, memory::allocator_deleter<T, memory::persistent_graphics_tagged_allocator<T>>>;
    // Type alias for a unique_ptr that uses the 'fileio' tag
    template <typename T>
    using fileio_unique_ptr = std::unique_ptr<T, memory::allocator_deleter<T, memory::persistent_fileio_tagged_allocator<T>>>;
    // Type alias for a unique_ptr that uses the 'stage' tag
    template <typename T>
    using stage_unique_ptr = std::unique_ptr<T, memory::allocator_deleter<T, memory::persistent_stage_tagged_allocator<T>>>;

    // Vectors
    // Type alias for a vector that uses the 'graphics' tag
    template <typename T>
    using graphics_vector = std::vector<T, memory::persistent_graphics_tagged_allocator<T>>;
    // Type alias for a vector that uses the 'fileio' tag
    template <typename T>
    using fileio_vector = std::vector<T, memory::persistent_fileio_tagged_allocator<T>>;
    // Type alias for a vector that uses the 'global' tag
    template <typename T>
    using global_vector = std::vector<T, memory::persistent_global_tagged_allocator<T>>;
    // Type alias for a vector that uses the 'stage' tag
    template <typename T>
    using stage_vector = std::vector<T, memory::persistent_stage_tagged_allocator<T>>;

    // Type alias for a vector that uses the frame allocator
    template <typename T>
    using temp_vector = std::vector<T, memory::persistent_frame_allocator<T>>;
    // Type alias for a vector that uses the scratch allocator
    template <typename T>
    using scratch_vector = std::vector<T, memory::persistent_scratch_allocator<T>>;

    // Type alias for a vector that uses the 'graphics' tag
    using graphics_blob = std::vector<std::byte, memory::persistent_graphics_tagged_allocator<std::byte>>;
    // Type alias for a vector that uses the 'fileio' tag
    using fileio_blob = std::vector<std::byte, memory::persistent_fileio_tagged_allocator<std::byte>>;
    // Type alias for a vector that uses the 'global' tag
    using global_blob = std::vector<std::byte, memory::persistent_global_tagged_allocator<std::byte>>;
    // Type alias for a vector that uses the 'stage' tag
    using stage_blob = std::vector<std::byte, memory::persistent_stage_tagged_allocator<std::byte>>;

    // Type alias for a vector that uses the frame allocator
    using temp_blob = std::vector<std::byte, memory::persistent_frame_allocator<std::byte>>;
    // Type alias for a vector that uses the scratch allocator
    using scratch_blob = std::vector<std::byte, memory::persistent_scratch_allocator<std::byte>>;

    // Hash maps
    // Type alias for a unordered_map that uses the 'graphics' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using graphics_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::persistent_graphics_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a unordered_map that uses the 'filio' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using fileio_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::persistent_fileio_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a unordered_map that uses the 'global' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using global_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::persistent_global_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a unordered_map that uses the 'stage' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using stage_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::persistent_stage_tagged_allocator<std::pair<const TKey, TValue>>>;

    // Type alias for a unordered_map that uses the frame allocator
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using temp_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::persistent_frame_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a unordered_map that uses the scratch allocator
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using scratch_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::persistent_scratch_allocator<std::pair<const TKey, TValue>>>;

    // Maps
    // Type alias for a map that uses the 'graphics' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using graphics_map = std::map<TKey, TValue, TPr, memory::persistent_graphics_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a map that uses the 'filio' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using fileio_map = std::map<TKey, TValue, TPr, memory::persistent_fileio_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a map that uses the 'global' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using global_map = std::map<TKey, TValue, TPr, memory::persistent_global_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a map that uses the 'stage' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using stage_map = std::map<TKey, TValue, TPr, memory::persistent_stage_tagged_allocator<std::pair<const TKey, TValue>>>;

    // Type alias for a map that uses the frame allocator
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using temp_map = std::map<TKey, TValue, TPr, memory::persistent_frame_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a map that uses the scratch allocator
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using scratch_map = std::map<TKey, TValue, TPr, memory::persistent_scratch_allocator<std::pair<const TKey, TValue>>>;

    // Sets
    // Type alias for a unordered_set that uses the 'graphics' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using graphics_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::persistent_graphics_tagged_allocator<TKey>>;
    // Type alias for a unordered_set that uses the 'filio' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using fileio_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::persistent_fileio_tagged_allocator<TKey>>;
    // Type alias for a unordered_set that uses the 'global' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using global_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::persistent_global_tagged_allocator<TKey>>;
    // Type alias for a unordered_set that uses the 'global' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using stage_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::persistent_stage_tagged_allocator<TKey>>;

    // Type alias for a unordered_set that uses the frame allocator
    template<typename TKey, typename THasher = std::hash<TKey>>
    using temp_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::persistent_frame_allocator<TKey>>;
    // Type alias for a unordered_set that uses the scratch allocator
    template<typename TKey, typename THasher = std::hash<TKey>>
    using scratch_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::persistent_scratch_allocator<TKey>>;

    // String Streams
    // Type alias for a basic_ostringstream that uses the 'graphics' tag
    using graphics_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::persistent_graphics_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'fileio' tag
    using fileio_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::persistent_fileio_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'global' tag
    using global_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::persistent_global_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'stage' tag
    using stage_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::persistent_stage_tagged_allocator<char>>;

    // Type alias for a basic_ostringstream that uses the frame allocator
    using temp_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::persistent_frame_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the scratch allocator
    using scratch_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::persistent_scratch_allocator<char>>;

    // Type alias for a basic_ostringstream that uses the 'graphics' tag
    using graphics_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::persistent_graphics_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'fileio' tag
    using fileio_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::persistent_fileio_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'global' tag
    using global_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::persistent_global_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'stage' tag
    using stage_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::persistent_stage_tagged_allocator<char>>;

    // Type alias for a basic_istringstream that uses the frame allocator
    using temp_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::persistent_frame_allocator<char>>;
    // Type alias for a basic_istringstream that uses the scratch allocator
    using scratch_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::persistent_scratch_allocator<char>>;

    // Type alias for a basic_ostringstream that uses the 'graphics' tag
    using graphics_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::persistent_graphics_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'fileio' tag
    using fileio_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::persistent_fileio_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'global' tag
    using global_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::persistent_global_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'stage' tag
    using stage_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::persistent_stage_tagged_allocator<char>>;

    // Type alias for a basic_stringstream that uses the frame allocator
    using temp_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::persistent_frame_allocator<char>>;
    // Type alias for a basic_stringstream that uses the scratch allocator
    using scratch_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::persistent_scratch_allocator<char>>;

    // Strings
    // Type alias for a basic_string that uses the 'graphics' tag
    using graphics_string = std::basic_string<char, std::char_traits<char>, memory::persistent_graphics_tagged_allocator<char>>;
    // Type alias for a basic_string that uses the 'fileio' tag
    using fileio_string = std::basic_string<char, std::char_traits<char>, memory::persistent_fileio_tagged_allocator<char>>;
    // Type alias for a basic_string that uses the 'global' tag
    using global_string = std::basic_string<char, std::char_traits<char>, memory::persistent_global_tagged_allocator<char>>;
    // Type alias for a basic_string that uses the 'stage' tag
    using stage_string = std::basic_string<char, std::char_traits<char>, memory::persistent_stage_tagged_allocator<char>>;

    // Type alias for a basic_string that uses the frame allocator
    using temp_string = std::basic_string<char, std::char_traits<char>, memory::persistent_frame_allocator<char>>;
    // Type alias for a basic_string that uses the scratch allocator
    using scratch_string = std::basic_string<char, std::char_traits<char>, memory::persistent_scratch_allocator<char>>;

    //-----------------------------------------------------------------------------
    // 
    // STAGING ALLOCATORS
    // 
    //-----------------------------------------------------------------------------

    namespace memory
    {
        // Allocator using the staging tagged heap.
        template<typename T>
        using transient_graphics_tagged_allocator = memory::tagged_heap_allocator<T, transient_tagged_policy, memory::tags::graphics>;
        template<typename T>
        using transient_fileio_tagged_allocator = memory::tagged_heap_allocator<T, transient_tagged_policy, memory::tags::fileio>;
        template<typename T>
        using transient_global_tagged_allocator = memory::tagged_heap_allocator<T, transient_tagged_policy, memory::tags::global>;
        template<typename T>
        using transient_stage_tagged_allocator = memory::tagged_heap_allocator<T, transient_tagged_policy, memory::tags::stage>;
        // Type aliases for staging allocators.
        template<typename T>
        using transient_global_allocator = memory::generic_heap_allocator<T, transient_global_policy>;
        template<typename T>
        using transient_frame_allocator = memory::generic_heap_allocator<T, transient_frame_policy>;
        template<typename T>
        using transient_scratch_allocator = memory::generic_heap_allocator<T, transient_scratch_policy>;
    }

    // Unique Ptr
    // Type alias for a unique_ptr that uses the 'global' tag
    template <typename T>
    using transient_global_unique_ptr = std::unique_ptr<T, memory::allocator_deleter<T, memory::transient_global_tagged_allocator<T>>>;
    // Type alias for a unique_ptr that uses the 'graphis' tag
    template <typename T>
    using transient_graphics_unique_ptr = std::unique_ptr<T, memory::allocator_deleter<T, memory::transient_graphics_tagged_allocator<T>>>;
    // Type alias for a unique_ptr that uses the 'fileio' tag
    template <typename T>
    using transient_fileio_unique_ptr = std::unique_ptr<T, memory::allocator_deleter<T, memory::transient_fileio_tagged_allocator<T>>>;
    // Type alias for a unique_ptr that uses the 'stage' tag
    template <typename T>
    using transient_stage_unique_ptr = std::unique_ptr<T, memory::allocator_deleter<T, memory::transient_stage_tagged_allocator<T>>>;

    // Vectors
    // Type alias for a vector that uses the 'geometry' tag
    template <typename T>
    using transient_graphics_vector = std::vector<T, memory::transient_graphics_tagged_allocator<T>>;
    // Type alias for a vector that uses the 'fileio' tag
    template <typename T>
    using transient_fileio_vector = std::vector<T, memory::transient_fileio_tagged_allocator<T>>;
    // Type alias for a vector that uses the 'global' tag
    template <typename T>
    using transient_global_vector = std::vector<T, memory::transient_global_tagged_allocator<T>>;
    // Type alias for a vector that uses the 'stage' tag
    template <typename T>
    using transient_stage_vector = std::vector<T, memory::transient_stage_tagged_allocator<T>>;

    // Type alias for a vector that uses the frame allocator
    template <typename T>
    using transient_temp_vector = std::vector<T, memory::transient_frame_allocator<T>>;
    // Type alias for a vector that uses the scratch allocator
    template <typename T>
    using transient_scratch_vector = std::vector<T, memory::transient_scratch_allocator<T>>;

    // Type alias for a vector that uses the 'graphics' tag
    using transient_graphics_blob = std::vector<std::byte, memory::transient_graphics_tagged_allocator<std::byte>>;
    // Type alias for a vector that uses the 'fileio' tag
    using transient_fileio_blob = std::vector<std::byte, memory::transient_fileio_tagged_allocator<std::byte>>;
    // Type alias for a vector that uses the 'global' tag
    using transient_global_blob = std::vector<std::byte, memory::transient_global_tagged_allocator<std::byte>>;
    // Type alias for a vector that uses the 'stage' tag
    using transient_stage_blob = std::vector<std::byte, memory::transient_stage_tagged_allocator<std::byte>>;

    // Type alias for a vector that uses the frame allocator
    using transient_temp_blob = std::vector<std::byte, memory::transient_frame_allocator<std::byte>>;
    // Type alias for a vector that uses the scratch allocator
    using transient_scratch_blob = std::vector<std::byte, memory::transient_scratch_allocator<std::byte>>;

    // Hash maps
    // Type alias for a unordered_map that uses the 'graphics' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using transient_graphics_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::transient_graphics_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a unordered_map that uses the 'filio' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using transient_fileio_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::transient_fileio_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a unordered_map that uses the 'global' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using transient_global_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::transient_global_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a unordered_map that uses the 'stage' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using transient_stage_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::transient_stage_tagged_allocator<std::pair<const TKey, TValue>>>;

    // Type alias for a unordered_map that uses the frame allocator
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using transient_temp_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::transient_frame_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a unordered_map that uses the scratch allocator
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using transient_scratch_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::transient_scratch_allocator<std::pair<const TKey, TValue>>>;

    // Maps
    // Type alias for a map that uses the 'graphics' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using transient_graphics_map = std::map<TKey, TValue, TPr, memory::transient_graphics_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a map that uses the 'filio' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using transient_fileio_map = std::map<TKey, TValue, TPr, memory::transient_fileio_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a map that uses the 'global' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using transient_global_map = std::map<TKey, TValue, TPr, memory::transient_global_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a map that uses the 'stage' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using transient_stage_map = std::map<TKey, TValue, TPr, memory::transient_stage_tagged_allocator<std::pair<const TKey, TValue>>>;

    // Type alias for a map that uses the frame allocator
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using transient_temp_map = std::map<TKey, TValue, TPr, memory::transient_frame_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a map that uses the scratch allocator
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using transient_scratch_map = std::map<TKey, TValue, TPr, memory::transient_scratch_allocator<std::pair<const TKey, TValue>>>;

    // Sets
    // Type alias for a unordered_set that uses the 'graphics' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using transient_graphics_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::transient_graphics_tagged_allocator<TKey>>;
    // Type alias for a unordered_set that uses the 'filio' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using transient_fileio_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::transient_fileio_tagged_allocator<TKey>>;
    // Type alias for a unordered_set that uses the 'global' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using transient_global_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::transient_global_tagged_allocator<TKey>>;
    // Type alias for a unordered_set that uses the 'stage' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using transient_stage_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::transient_stage_tagged_allocator<TKey>>;

    // Type alias for a unordered_set that uses the frame allocator
    template<typename TKey, typename THasher = std::hash<TKey>>
    using transient_temp_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::transient_frame_allocator<TKey>>;
    // Type alias for a unordered_set that uses the scratch allocator
    template<typename TKey, typename THasher = std::hash<TKey>>
    using transient_scratch_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::transient_scratch_allocator<TKey>>;

    // String Streams
    // Type alias for a basic_ostringstream that uses the 'graphics' tag
    using transient_graphics_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::transient_graphics_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'fileio' tag
    using transient_fileio_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::transient_fileio_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'global' tag
    using transient_global_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::transient_global_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'stage' tag
    using transient_stage_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::transient_stage_tagged_allocator<char>>;

    // Type alias for a basic_ostringstream that uses the frame allocator
    using transient_temp_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::transient_frame_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the scratch allocator
    using transient_scratch_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::transient_scratch_allocator<char>>;

    // Type alias for a basic_ostringstream that uses the 'graphics' tag
    using transient_graphics_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::transient_graphics_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'fileio' tag
    using transient_fileio_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::transient_fileio_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'global' tag
    using transient_global_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::transient_global_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'stage' tag
    using transient_stage_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::transient_stage_tagged_allocator<char>>;

    // Type alias for a basic_istringstream that uses the frame allocator
    using transient_temp_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::transient_frame_allocator<char>>;
    // Type alias for a basic_istringstream that uses the scratch allocator
    using transient_scratch_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::transient_scratch_allocator<char>>;

    // Type alias for a basic_ostringstream that uses the 'graphics' tag
    using transient_graphics_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::transient_graphics_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'fileio' tag
    using transient_fileio_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::transient_fileio_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'global' tag
    using transient_global_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::transient_global_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'stage' tag
    using transient_stage_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::transient_stage_tagged_allocator<char>>;

    // Type alias for a basic_stringstream that uses the frame allocator
    using transient_temp_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::transient_frame_allocator<char>>;
    // Type alias for a basic_stringstream that uses the scratch allocator
    using transient_scratch_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::transient_scratch_allocator<char>>;

    // Strings
    // Type alias for a basic_string that uses the 'graphics' tag
    using transient_graphics_string = std::basic_string<char, std::char_traits<char>, memory::transient_graphics_tagged_allocator<char>>;
    // Type alias for a basic_string that uses the 'fileio' tag
    using transient_fileio_string = std::basic_string<char, std::char_traits<char>, memory::transient_fileio_tagged_allocator<char>>;
    // Type alias for a basic_string that uses the 'global' tag
    using transient_global_string = std::basic_string<char, std::char_traits<char>, memory::transient_global_tagged_allocator<char>>;
    // Type alias for a basic_string that uses the 'stage' tag
    using transient_stage_string = std::basic_string<char, std::char_traits<char>, memory::transient_stage_tagged_allocator<char>>;

    // Type alias for a basic_string that uses the frame allocator
    using transient_temp_string = std::basic_string<char, std::char_traits<char>, memory::transient_frame_allocator<char>>;
    // Type alias for a basic_string that uses the scratch allocator
    using transient_scratch_string = std::basic_string<char, std::char_traits<char>, memory::transient_scratch_allocator<char>>;

    //-----------------------------------------------------------------------------
    // 
    // DEBUG ALLOCATORS
    // 
    //-----------------------------------------------------------------------------

    namespace memory
    {
        // Allocator using the debug tagged heap.
        template<typename T>
        using debug_graphics_tagged_allocator = memory::tagged_heap_allocator<T, debug_tagged_policy, memory::tags::graphics>;
        template<typename T>
        using debug_fileio_tagged_allocator = memory::tagged_heap_allocator<T, debug_tagged_policy, memory::tags::fileio>;
        template<typename T>
        using debug_global_tagged_allocator = memory::tagged_heap_allocator<T, debug_tagged_policy, memory::tags::global>;
        template<typename T>
        using debug_stage_tagged_allocator = memory::tagged_heap_allocator<T, debug_tagged_policy, memory::tags::stage>;
        // Type aliases for debug allocators.
        template<typename T>
        using debug_global_allocator = memory::generic_heap_allocator<T, debug_global_policy>;
        template<typename T>
        using debug_frame_allocator = memory::generic_heap_allocator<T, debug_frame_policy>;
        template<typename T>
        using debug_scratch_allocator = memory::generic_heap_allocator<T, debug_scratch_policy>;
    }
    
    // Unique Ptr
    // Type alias for a unique_ptr that uses the 'global' tag
    template <typename T>
    using debug_global_unique_ptr = std::unique_ptr<T, memory::allocator_deleter<T, memory::debug_global_tagged_allocator<T>>>;
    // Type alias for a unique_ptr that uses the 'graphics' tag
    template <typename T>
    using debug_graphics_unique_ptr = std::unique_ptr<T, memory::allocator_deleter<T, memory::debug_graphics_tagged_allocator<T>>>;
    // Type alias for a unique_ptr that uses the 'fileio' tag
    template <typename T>
    using debug_fileio_unique_ptr = std::unique_ptr<T, memory::allocator_deleter<T, memory::debug_fileio_tagged_allocator<T>>>;
    // Type alias for a unique_ptr that uses the 'stage' tag
    template <typename T>
    using debug_stage_unique_ptr = std::unique_ptr<T, memory::allocator_deleter<T, memory::debug_stage_tagged_allocator<T>>>;

    // Vectors
    // Type alias for a vector that uses the 'geometry' tag
    template <typename T>
    using debug_graphics_vector = std::vector<T, memory::debug_graphics_tagged_allocator<T>>;
    // Type alias for a vector that uses the 'fileio' tag
    template <typename T>
    using debug_fileio_vector = std::vector<T, memory::debug_fileio_tagged_allocator<T>>;
    // Type alias for a vector that uses the 'global' tag
    template <typename T>
    using debug_global_vector = std::vector<T, memory::debug_global_tagged_allocator<T>>;
    // Type alias for a vector that uses the 'stage' tag
    template <typename T>
    using debug_stage_vector = std::vector<T, memory::debug_stage_tagged_allocator<T>>;

    // Type alias for a vector that uses the frame allocator
    template <typename T>
    using debug_temp_vector = std::vector<T, memory::debug_frame_allocator<T>>;
    // Type alias for a vector that uses the scratch allocator
    template <typename T>
    using debug_scratch_vector = std::vector<T, memory::debug_scratch_allocator<T>>;

    // Type alias for a vector that uses the 'graphics' tag
    using debug_graphics_blob = std::vector<std::byte, memory::debug_graphics_tagged_allocator<std::byte>>;
    // Type alias for a vector that uses the 'fileio' tag
    using debug_fileio_blob = std::vector<std::byte, memory::debug_fileio_tagged_allocator<std::byte>>;
    // Type alias for a vector that uses the 'global' tag
    using debug_global_blob = std::vector<std::byte, memory::debug_global_tagged_allocator<std::byte>>;
    // Type alias for a vector that uses the 'stage' tag
    using debug_stage_blob = std::vector<std::byte, memory::debug_stage_tagged_allocator<std::byte>>;

    // Type alias for a vector that uses the frame allocator
    using debug_temp_blob = std::vector<std::byte, memory::debug_frame_allocator<std::byte>>;
    // Type alias for a vector that uses the scratch allocator
    using debug_scratch_blob = std::vector<std::byte, memory::debug_scratch_allocator<std::byte>>;

    // Hash maps
    // Type alias for a unordered_map that uses the 'graphics' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using debug_graphics_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::debug_graphics_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a unordered_map that uses the 'filio' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using debug_fileio_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::debug_fileio_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a unordered_map that uses the 'global' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using debug_global_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::debug_global_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a unordered_map that uses the 'stage' tag
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using debug_stage_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::debug_stage_tagged_allocator<std::pair<const TKey, TValue>>>;

    // Type alias for a unordered_map that uses the frame allocator
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using debug_temp_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::debug_frame_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a unordered_map that uses the scratch allocator
    template<typename TKey, typename TValue, typename THasher = std::hash<TKey>>
    using debug_scratch_hash_map = std::unordered_map<TKey, TValue, THasher, std::equal_to<TKey>, memory::debug_scratch_allocator<std::pair<const TKey, TValue>>>;

    // Maps
    // Type alias for a map that uses the 'graphics' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using debug_graphics_map = std::map<TKey, TValue, TPr, memory::debug_graphics_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a map that uses the 'filio' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using debug_fileio_map = std::map<TKey, TValue, TPr, memory::debug_fileio_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a map that uses the 'global' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using debug_global_map = std::map<TKey, TValue, TPr, memory::debug_global_tagged_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a map that uses the 'stage' tag
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using debug_stage_map = std::map<TKey, TValue, TPr, memory::debug_stage_tagged_allocator<std::pair<const TKey, TValue>>>;

    // Type alias for a map that uses the frame allocator
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using debug_temp_map = std::map<TKey, TValue, TPr, memory::debug_frame_allocator<std::pair<const TKey, TValue>>>;
    // Type alias for a map that uses the scratch allocator
    template<typename TKey, typename TValue, typename TPr = std::less<TKey>>
    using debug_scratch_map = std::map<TKey, TValue, TPr, memory::debug_scratch_allocator<std::pair<const TKey, TValue>>>;

    // Sets
    // Type alias for a unordered_set that uses the 'graphics' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using debug_graphics_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::debug_graphics_tagged_allocator<TKey>>;
    // Type alias for a unordered_set that uses the 'filio' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using debug_fileio_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::debug_fileio_tagged_allocator<TKey>>;
    // Type alias for a unordered_set that uses the 'global' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using debug_global_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::debug_global_tagged_allocator<TKey>>;
    // Type alias for a unordered_set that uses the 'stage' tag
    template<typename TKey, typename THasher = std::hash<TKey>>
    using debug_stage_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::debug_stage_tagged_allocator<TKey>>;

    // Type alias for a unordered_set that uses the frame allocator
    template<typename TKey, typename THasher = std::hash<TKey>>
    using debug_temp_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::debug_frame_allocator<TKey>>;
    // Type alias for a unordered_set that uses the scratch allocator
    template<typename TKey, typename THasher = std::hash<TKey>>
    using debug_scratch_set = std::unordered_set<TKey, THasher, std::equal_to<TKey>, memory::debug_scratch_allocator<TKey>>;

    // String Streams
    // Type alias for a basic_ostringstream that uses the 'graphics' tag
    using debug_graphics_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::debug_graphics_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'fileio' tag
    using debug_fileio_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::debug_fileio_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'global' tag
    using debug_global_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::debug_global_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'stage' tag
    using debug_stage_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::debug_stage_tagged_allocator<char>>;

    // Type alias for a basic_ostringstream that uses the frame allocator
    using debug_temp_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::debug_frame_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the scratch allocator
    using debug_scratch_ostringstream = std::basic_ostringstream<char, std::char_traits<char>, memory::debug_scratch_allocator<char>>;

    // Type alias for a basic_ostringstream that uses the 'graphics' tag
    using debug_graphics_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::debug_graphics_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'fileio' tag
    using debug_fileio_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::debug_fileio_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'global' tag
    using debug_global_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::debug_global_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'stage' tag
    using debug_stage_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::debug_stage_tagged_allocator<char>>;

    // Type alias for a basic_istringstream that uses the frame allocator
    using debug_temp_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::debug_frame_allocator<char>>;
    // Type alias for a basic_istringstream that uses the scratch allocator
    using debug_scratch_istringstream = std::basic_istringstream<char, std::char_traits<char>, memory::debug_scratch_allocator<char>>;

    // Type alias for a basic_ostringstream that uses the 'graphics' tag
    using debug_graphics_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::debug_graphics_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'fileio' tag
    using debug_fileio_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::debug_fileio_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'global' tag
    using debug_global_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::debug_global_tagged_allocator<char>>;
    // Type alias for a basic_ostringstream that uses the 'stage' tag
    using debug_stage_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::debug_stage_tagged_allocator<char>>;

    // Type alias for a basic_stringstream that uses the frame allocator
    using debug_temp_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::debug_frame_allocator<char>>;
    // Type alias for a basic_stringstream that uses the scratch allocator
    using debug_scratch_stringstream = std::basic_stringstream<char, std::char_traits<char>, memory::debug_scratch_allocator<char>>;

    // Strings
    // Type alias for a basic_string that uses the 'graphics' tag
    using debug_graphics_string = std::basic_string<char, std::char_traits<char>, memory::debug_graphics_tagged_allocator<char>>;
    // Type alias for a basic_string that uses the 'fileio' tag
    using debug_fileio_string = std::basic_string<char, std::char_traits<char>, memory::debug_fileio_tagged_allocator<char>>;
    // Type alias for a basic_string that uses the 'global' tag
    using debug_global_string = std::basic_string<char, std::char_traits<char>, memory::debug_global_tagged_allocator<char>>;
    // Type alias for a basic_string that uses the 'stage' tag
    using debug_stage_string = std::basic_string<char, std::char_traits<char>, memory::debug_stage_tagged_allocator<char>>;

    // Type alias for a basic_string that uses the frame allocator
    using debug_temp_string = std::basic_string<char, std::char_traits<char>, memory::debug_frame_allocator<char>>;
    // Type alias for a basic_string that uses the scratch allocator
    using debug_scratch_string = std::basic_string<char, std::char_traits<char>, memory::debug_scratch_allocator<char>>;
}
#pragma once

#include "rex_engine/memory/debug_allocator.h"
#include "rex_engine/memory/untracked_allocator.h"
#include "rex_std/bonus/types.h"
#include "rex_std/sstream.h"
#include "rex_std/string.h"
#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

using s8      = int8;
using u8      = uint8;
using s16     = int16;
using u16     = uint16;
using s32     = int32;
using u32     = uint32;
using s64     = int64;
using u64     = uint64;
using hash_id = uint32;

using char8 = rsl::char8;
using tchar = rsl::tchar;

using f32 = float;
using f64 = double;

using ulong = unsigned long;

// allow single threaded platforms to avoid use of atomic
#if REX_WEB || REX_WINDOWS
  #define REX_SINGLE_THREADED 1
#else
  #define REX_SINGLE_THREADED 0
#endif

#if REX_SINGLE_THREADED
using a_u8   = u8;
using a_u32  = u32;
using a_u64  = u64;
using a_bool = bool;
using a_s32  = s32;
  #define rex_atomic_load(a) a // NOLINT(readability-identifier-naming)
#else
  #include <atomic>
using a_u8   = std::atomic<u8>;
using a_u32  = std::atomic<u32>;
using a_u64  = std::atomic<u64>;
using a_bool = std::atomic<bool>;
using a_s32  = std::atomic<s32>;
  #define rex_atomic_load(a) a.load() // NOLINT(readability-identifier-naming)
#endif

namespace rex
{
  // if the string needs to be optimized further, make the Allocator type for the DebugAllocator a template,
  // and pass it as a template argument.
  // Strings usually won't need special allocators, at least not at the moment, but that might change in the future
  using DebugString       = rsl::basic_string<char8, rsl::char_traits<char8>, rex::DebugAllocator<rex::UntrackedAllocator>>;
  using DebugStringStream = rsl::basic_istringstream<char8, rsl::char_traits<char8>, rex::DebugAllocator<rex::UntrackedAllocator>>;

  template <typename T, typename Allocator = rex::UntrackedAllocator>
  using DebugVector = rsl::vector<T, rex::DebugAllocator<Allocator>>;
  template <typename Key, typename Value, typename Hash = rsl::hash<Key>, typename EqualTo = rsl::equal_to<Key>, typename Allocator = rex::UntrackedAllocator>
  using DebugHashTable = rsl::unordered_map<Key, Value, Hash, EqualTo, rex::DebugAllocator<Allocator>>;
} // namespace rex

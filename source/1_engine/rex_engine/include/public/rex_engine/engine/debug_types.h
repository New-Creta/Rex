#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/global_allocators/global_debug_allocator.h"
#include "rex_engine/memory/allocators/untracked_allocator.h"
#include "rex_std/sstream.h"
#include "rex_std/string.h"
#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

namespace rex
{
  // if the string needs to be optimized further, make the Allocator type for the DebugAllocator a template,
  // and pass it as a template argument.
  // Strings usually won't need special allocators, at least not at the moment, but that might change in the future
  using DebugString       = rsl::basic_string<char8, rsl::char_traits<char8>, rex::DebugAllocator<rex::UntrackedAllocator>>;
  using DebugWString      = rsl::basic_string<tchar, rsl::char_traits<tchar>, rex::DebugAllocator<rex::UntrackedAllocator>>;
  using DebugStringStream = rsl::basic_stringstream<char8, rsl::char_traits<char8>, rex::DebugAllocator<rex::UntrackedAllocator>>;

  template <typename T, typename Allocator = rex::UntrackedAllocator>
  using DebugVector = rsl::vector<T, rex::DebugAllocator<Allocator>>;
  template <typename Key, typename Value, typename Hash = rsl::hash<Key>, typename EqualTo = rsl::equal_to<Key>, typename Allocator = rex::UntrackedAllocator>
  using DebugHashTable = rsl::unordered_map<Key, Value, Hash, EqualTo, rex::DebugAllocator<Allocator>>;
} // namespace rex

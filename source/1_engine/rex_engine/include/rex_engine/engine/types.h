#pragma once

#include "rex_std/atomic.h"
#include "rex_std/bonus/types.h"
#include "rex_std/bonus/utility/strong_type.h"

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
using a_u8   = rsl::atomic<u8>;
using a_u32  = rsl::atomic<u32>;
using a_u64  = rsl::atomic<u64>;
using a_bool = rsl::atomic<bool>;
using a_s32  = rsl::atomic<s32>;
  #define rex_atomic_load(a) a.load() // NOLINT(readability-identifier-naming)
#endif

namespace rex
{
  namespace globals
  {
    constexpr s32 g_invalid_slot_id = -1;
  } // namespace globals
} // namespace rex

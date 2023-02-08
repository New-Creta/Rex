#pragma once

#include <stdint.h>

using s8      = int8_t;
using u8      = uint8_t;
using s16     = int16_t;
using u16     = uint16_t;
using s32     = int32_t;
using u32     = uint32_t;
using s64     = int64_t;
using u64     = uint64_t;
using hash_id = uint32_t;

using char8 = char;
using tchar = wchar_t;

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
using a_u8 = u8;
using a_u32 = u32;
using a_u64 = u64;
using a_bool = bool;
using a_s32 = s32;
#define rex_atomic_load(a) a // NOLINT(readability-identifier-naming)
#else
  #include <atomic>
using a_u8 = std::atomic<u8>;
using a_u32 = std::atomic<u32>;
using a_u64 = std::atomic<u64>;
using a_bool = std::atomic<bool>;
using a_s32 = std::atomic<s32>;
  #define rex_atomic_load(a) a.load() // NOLINT(readability-identifier-naming)
#endif
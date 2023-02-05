#pragma once

#include <stdint.h>

using s8 = int8_t;
using u8 = uint8_t;
using s16 = int16_t;
using u16 = uint16_t;
using s32 = int32_t;
using u32 = uint32_t;
using s64 = int64_t;
using u64 = uint64_t;
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
typedef u8     a_u8;
typedef u32    a_u32;
typedef u64    a_u64;
typedef bool   a_bool;
typedef s32    a_s32;
#define rex_atomic_load(a) a
#else
#include <atomic>
typedef std::atomic<uint8_t>  a_u8;
typedef std::atomic<uint32_t> a_u32;
typedef std::atomic<uint64_t> a_u64;
typedef std::atomic<bool>     a_bool;
typedef std::atomic<s32>      a_s32;
#define rex_atomic_load(a) a.load()
#endif
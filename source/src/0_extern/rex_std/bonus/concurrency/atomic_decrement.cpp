// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: atomic_decrement.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/bonus/concurrency/atomic_decrement.h"

#include <Windows.h>

/// [06/Aug/2022] RSL Comment: We wrap the functions in internal calls to avoid C28113 on MSVC
namespace rsl
{
  inline namespace v1
  {
    namespace internal
    {
      int16 atomic_decrement(int16* data)
      {
        return _InterlockedDecrement16(data);
      }
      int32 atomic_decrement(long* data)
      {
        return _InterlockedDecrement(data);
      }
      int64 atomic_decrement(int64* data)
      {
        return _InterlockedDecrement64(data);
      }
      uint16 atomic_decrement(uint16* data)
      {
        return static_cast<uint16>(atomic_decrement(reinterpret_cast<int16*>(data))); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
      }
      uint32 atomic_decrement(ulong* data)
      {
        return _InterlockedDecrement(data);
      }
      uint64 atomic_decrement(uint64* data)
      {
        return static_cast<uint64>(atomic_decrement(reinterpret_cast<int64*>(data))); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
      }
    } // namespace internal
  }   // namespace v1
} // namespace rsl

int8 rsl::atomic_decrement(int8& val)
{
  long i32_val = val; // NOLINT(bugprone-signed-char-misuse)
  return static_cast<int8>(internal::atomic_decrement(&i32_val));
}
int16 rsl::atomic_decrement(int16& val)
{
  return internal::atomic_decrement(&val);
}
int32 rsl::atomic_decrement(int32& val)
{
  return internal::atomic_decrement(reinterpret_cast<long*>(&val)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
}
int64 rsl::atomic_decrement(int64& val)
{
  return internal::atomic_decrement(&val);
}

uint8 rsl::atomic_decrement(uint8& val)
{
  ulong u32_val = val;
  return static_cast<uint8>(internal::atomic_decrement(&u32_val));
}
uint16 rsl::atomic_decrement(uint16& val)
{
  return internal::atomic_decrement(&val);
}
uint32 rsl::atomic_decrement(uint32& val)
{
  return internal::atomic_decrement(reinterpret_cast<ulong*>(&val)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
}
uint64 rsl::atomic_decrement(uint64& val)
{
  return internal::atomic_decrement(&val);
}

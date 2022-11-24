// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: atomic_increment.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/bonus/concurrency/atomic_increment.h"

#include <Windows.h>

/// [06/Aug/2022] RSL Comment: We wrap the functions in internal calls to avoid C28113 on MSVC
namespace rsl
{
  inline namespace v1
  {
    namespace internal
    {
      int16 atomic_increment(int16* data)
      {
        return _InterlockedIncrement16(data);
      }
      int32 atomic_increment(long* data)
      {
        return _InterlockedIncrement(data);
      }
      int64 atomic_increment(int64* data)
      {
        return _InterlockedIncrement64(data);
      }
      uint16 atomic_increment(uint16* data)
      {
        return static_cast<uint16>(atomic_increment(reinterpret_cast<int16*>(data))); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
      }
      uint32 atomic_increment(ulong* data)
      {
        return _InterlockedIncrement(data);
      }
      uint64 atomic_increment(uint64* data)
      {
        return static_cast<uint64>(atomic_increment(reinterpret_cast<int64*>(data))); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
      }
    } // namespace internal
  }   // namespace v1
} // namespace rsl

int8 rsl::atomic_increment(int8& val)
{
  long i32_val = val; // NOLINT(bugprone-signed-char-misuse)
  return static_cast<int8>(internal::atomic_increment(&i32_val));
}
int16 rsl::atomic_increment(int16& val)
{
  return internal::atomic_increment(&val);
}
int32 rsl::atomic_increment(int32& val)
{
  return internal::atomic_increment(reinterpret_cast<long*>(&val)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
}
int64 rsl::atomic_increment(int64& val)
{
  return internal::atomic_increment(&val);
}

uint8 rsl::atomic_increment(uint8& val)
{
  ulong u32_val = val;
  return static_cast<uint8>(internal::atomic_increment(&u32_val));
}
uint16 rsl::atomic_increment(uint16& val)
{
  return internal::atomic_increment(&val);
}
uint32 rsl::atomic_increment(uint32& val)
{
  return internal::atomic_increment(reinterpret_cast<ulong*>(&val)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
}
uint64 rsl::atomic_increment(uint64& val)
{
  return internal::atomic_increment(&val);
}
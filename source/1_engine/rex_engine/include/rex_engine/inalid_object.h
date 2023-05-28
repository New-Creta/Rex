#pragma once

#include "rex_engine/memory/pointer_math.h"
#include "rex_engine/types.h"
#include "rex_std/array.h"

namespace rex
{
  template <typename T>
  T invalid_obj()
  {
    constexpr s32 invalid_value_marker = 0xDEAD; // == 57005
    constexpr size_t object_size       = align(sizeof(T), alignof(int));
    rsl::array<char, object_size> buff;
    // to avoid weird issues, intstead of using fill member func
    // we use memset, as we know that the buffer will have n block of 4 bytes
    std::memset(buff.data(), invalid_value_marker, object_size / sizeof(invalid_value_marker));
    return *reinterpret_cast<T*>(buff.data());
  }
} // namespace rex
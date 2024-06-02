#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/pointer_math.h"
#include "rex_std/array.h"

namespace rex
{
  template <typename T>
  T invalid_obj()
  {
    constexpr s32 invalid_value_marker = 0xDEAD; // == 57005
    constexpr size_t object_size       = align(sizeof(T), alignof(s32));
    rsl::array<s32, object_size / sizeof(invalid_value_marker)> buff;
    buff.fill(invalid_value_marker);
    return *reinterpret_cast<T*>(buff.data());
  }
} // namespace rex
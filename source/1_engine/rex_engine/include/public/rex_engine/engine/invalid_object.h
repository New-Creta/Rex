#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/pointer_math.h"
#include "rex_std/array.h"

namespace rex
{
  constexpr s32 s_invalid_obj_value = 0xDEAD; // == 57005

  template <typename T>
  constexpr T invalid_obj()
  {
    constexpr size_t object_size       = align(sizeof(T), alignof(s32));
    rsl::array<s32, object_size / sizeof(s_invalid_obj_value)> buff;
    buff.fill(s_invalid_obj_value);
    return *reinterpret_cast<T*>(buff.data());
  }
} // namespace rex
#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/pointer_math.h"
#include "rex_std/array.h"
#include "rex_std/memory.h"
#include "rex_std/algorithm.h"
#include "rex_std/iterator.h"

namespace rex
{
  namespace internal
  {
    constexpr s32 s_invalid_obj_value = 0xDEAD; // == 57005
  }

  // Return a data array of size of T, aligned to size of s32
  // Fill the array with repeated values of s_invalid_obj_value
  template <typename T>
  constexpr T invalid_obj()
  {
    constexpr size_t object_size       = align(sizeof(T), alignof(s32));
    rsl::array<s32, object_size / sizeof(rex::internal::s_invalid_obj_value)> buff;
    buff.fill(rex::internal::s_invalid_obj_value);
    return *reinterpret_cast<T*>(buff.data());
  }

  // Verify if an object is valid
  template <typename T>
  bool is_invalid(const T& obj)
  {
    // If we're not aligned, we're not invalid
    if (sizeof(obj) % sizeof(rex::internal::s_invalid_obj_value) != 0)
    {
      return false;
    }

    const s32 data_array = reinterpret_cast<const s32*>(rsl::addressof(obj));
    
    return rsl::all_of(rsl::begin(data_array), rsl::end(data_array), [](s32 x) { return x == internal::s_invalid_obj_value; });
  }
} // namespace rex
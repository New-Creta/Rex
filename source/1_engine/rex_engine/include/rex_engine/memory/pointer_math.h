#pragma once

#include "rex_std/bonus/types.h"

namespace rex
{
  constexpr void* jump_forward(void* mem, count_t count)
  {
    rsl::byte* mem_as_bytes = static_cast<rsl::byte*>(mem);
    mem_as_bytes += count;
    return mem_as_bytes;
  }
  constexpr void* jump_backward(void* mem, count_t count)
  {
    rsl::byte* mem_as_bytes = static_cast<rsl::byte*>(mem);
    mem_as_bytes -= count;
    return mem_as_bytes;
  }
}
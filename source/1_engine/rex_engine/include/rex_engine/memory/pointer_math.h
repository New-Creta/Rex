#pragma once

#include "rex_engine/diagnostics/assert.h"
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
  constexpr uintptr align(uintptr addr, card32 alignment)
  {
    const card32 mask = alignment - 1;
    REX_ASSERT_X((alignment & mask) == 0, "Alignment must be a power of 2"); // NOLINT(hicpp-signed-bitwise)
    return (addr + mask) & ~mask;                                            // NOLINT(hicpp-signed-bitwise)
  }
} // namespace rex
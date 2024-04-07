#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/types.h"
#include "rex_std/limits.h"

namespace rex
{
  constexpr card64 merge_int32_to_int64(card32 high, card32 low)
  {
    card64 res = {};
    res        = high;
    res <<= sizeof(high) * static_cast<u32>(rsl::limits_byte::num_bits_per_byte); // NOLINT(hicpp-signed-bitwise)
    res |= static_cast<u32>(low); // NOLINT(hicpp-signed-bitwise)

    return res;
  }

  constexpr u64 merge_int32_to_int64(u32 high, u32 low)
  {
    return merge_int32_to_int64(static_cast<s32>(high), static_cast<s32>(low));
  }
  constexpr u64 merge_int32_to_int64(ulong high, ulong low)
  {
    return merge_int32_to_int64(static_cast<s32>(high), static_cast<s32>(low));
  }
} // namespace rex
#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/types.h"
#include "rex_std/limits.h"

namespace rex
{
  constexpr u64 merge_int32_to_int64(u32 high, u32 low)
  {
    u64 res = {};
    res = high;
    res <<= sizeof(high) * static_cast<u32>(rsl::limits_byte::num_bits_per_byte);
    res |= static_cast<u32>(low);

    return res;
  }
  constexpr s64 merge_int32_to_int64(s32 high, s32 low)
  {
    return static_cast<s64>(merge_int32_to_int64(static_cast<u32>(high), static_cast<u32>(low)));
  }

  constexpr u64 merge_int32_to_int64(ulong high, ulong low)
  {
    return merge_int32_to_int64(static_cast<u32>(high), static_cast<u32>(low));
  }

  struct SplitIntResult
  {
    u32 high;
    u32 low;
  };
  constexpr SplitIntResult split_int64_into_int32(u64 input)
  {
    SplitIntResult res{};
    res.low = input & (rsl::numeric_limits<u32>::max)();
    input >>= sizeof(res.low) * static_cast<u32>(rsl::limits_byte::num_bits_per_byte);
    res.high = input & (rsl::numeric_limits<u32>::max)();

    return res;
  }
} // namespace rex
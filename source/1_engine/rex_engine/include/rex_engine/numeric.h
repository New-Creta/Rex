#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/limits.h"

namespace rex
{
  constexpr card64 merge_int32_to_int64(card32 high, card32 low)
  {
    card64 res = {};
    res        = high;
    res <<= sizeof(high) * rsl::limits_byte::num_bits_per_byte;
    res |= low;

    return res;
  }
} // namespace rex
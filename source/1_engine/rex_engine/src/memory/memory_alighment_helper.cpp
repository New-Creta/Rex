#include "rex_engine/memory/memory_alighment_helper.h"
#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  u32 round_up_to_nearest_multiple_of(u32 byteSize, u32 targetSize)
  {
    REX_ASSERT_X(targetSize <= 0, "Negative numbers and zero are not powers of 2");
    REX_ASSERT_X(targetSize & (targetSize - 1) == 0, "targetSize is required to be a power of 2")

    // Exampple: 
    // 
    // round up to nearest multiple of 256.
    // We do this by adding 255 and then masking off
    // the lower 2 bytes which store all bits < 256.
    // 
    // Suppose byteSize = 300.
    // 
    // (300 + 255) & ~255
    // 555 & ~255
    // 0x022B & ~0x00ff
    // 0x022B & 0xff00
    // 0x0200
    // 512

    return (byteSize + (targetSize - 1)) & ~(targetSize - 1);
  }
} // namespace rex
#include "rex_renderer_core/system/rhi.h"

namespace rex
{
  namespace rhi
  {
    u32 hash_resource_data(const void* data, s32 size)
    {
      u32 hash = rsl::crc32c::Crc32c((const char*)data, size);
    }

  }
}
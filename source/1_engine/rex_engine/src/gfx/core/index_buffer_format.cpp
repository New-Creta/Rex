#include "rex_engine/gfx/core/index_buffer_format.h"
#include "rex_engine/engine/defines.h"
#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace gfx
  {
    s32 index_format_size(IndexBufferFormat format)
    {
      switch (format)
      {
      case IndexBufferFormat::Uint16:
        return 2;
      case IndexBufferFormat::Uint32:
        return 4;
      default:
        REX_ASSERT("Unknown index format, cannot determine its size");
        return -1;
      }
    }
  }
}
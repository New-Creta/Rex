#include "rex_renderer_core/rendering/index_buffer_format.h"
#include "rex_engine/engine/defines.h"
#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace renderer
  {
    s32 index_format_size(IndexBufferFormat format)
    {
      switch (format)
      {
      case rex::renderer::IndexBufferFormat::Uint16:
        return 2;
      case rex::renderer::IndexBufferFormat::Uint32:
        return 4;
      default:
        REX_ASSERT("Unknown index format");
        return 0;
      }
    }
  }
}
#pragma once

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/defines.h"

namespace rex
{
  namespace renderer
  {
    enum class IndexBufferFormat
    {
      None = 0,
      Uint16,
      Uint32
    };

    REX_FORCE_INLINE s32 index_format_size(IndexBufferFormat format)
    {
      switch(format)
      {
        case rex::renderer::IndexBufferFormat::Uint16: return 2;
        case rex::renderer::IndexBufferFormat::Uint32: return 4;
        default: REX_ASSERT("Unknown index format"); return 0;
      }
    }
  } // namespace renderer
} // namespace rex
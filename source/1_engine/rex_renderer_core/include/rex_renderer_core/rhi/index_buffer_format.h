#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace renderer
  {
    // graphics api abstraction of the index buffer format
    enum class IndexBufferFormat
    {
      None = 0, // None/Unknown format
      Uint16,   // An index is 16 bits
      Uint32    // An index is 32 bits
    };

    // Return the size in bytes of an index buffer format
    s32 index_format_size(IndexBufferFormat format);
  } // namespace renderer
} // namespace rex
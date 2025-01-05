#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/blob_view.h"
#include "rex_engine/gfx/core/index_buffer_format.h"
#include "rex_engine/gfx/resources/buffer.h"

namespace rex
{
  namespace gfx
  {
    // A buffer holding the indices for a primitive to render to the gpu
    class IndexBuffer : public Buffer
    {
    public:
      IndexBuffer(s32 indexCount, IndexBufferFormat format);

      // Return the number of indices in the buffer
      s32 count() const;
      // Return the size of a single index
      s32 index_byte_size() const;
      // Return the total size of the buffer
      s32 total_size() const;
      // Return the format of an index in the buffer
      IndexBufferFormat format() const;

    private:
      s32 m_index_count;                    // number of indices in the buffer
      IndexBufferFormat m_format; // format of an index
    };

  } // namespace gfx
} // namespace rex
#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/blob_view.h"
#include "rex_renderer_core/rhi/index_buffer_format.h"
#include "rex_renderer_core/resources/buffer.h"

namespace rex
{
  namespace rhi
  {
    // A buffer holding the indices for a primitive to render to the gpu
    class IndexBuffer : public Buffer
    {
    public:
      IndexBuffer(s32 indexCount, renderer::IndexBufferFormat format);

      // Return the number of indices in the buffer
      s32 count() const;
      // Return the size of a single index
      s32 index_byte_size() const;
      // Return the total size of the buffer
      s32 total_size() const;
      // Return the format of an index in the buffer
      renderer::IndexBufferFormat format() const;

    private:
      s32 m_index_count;                    // number of indices in the buffer
      renderer::IndexBufferFormat m_format; // format of an index
    };

  } // namespace rhi
} // namespace rex
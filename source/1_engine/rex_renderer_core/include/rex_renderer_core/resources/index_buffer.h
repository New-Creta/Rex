#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/blob_view.h"
#include "rex_renderer_core/rhi/index_buffer_format.h"
#include "rex_renderer_core/resources/buffer.h"

namespace rex
{
  namespace rhi
  {
    class IndexBuffer : public Buffer
    {
    public:
      IndexBuffer(s32 indexCount, renderer::IndexBufferFormat format);

      s32 count() const;
      s32 index_byte_size() const;
      s32 total_size() const;
      renderer::IndexBufferFormat format() const;

    private:
      s32 m_index_count;
      renderer::IndexBufferFormat m_format;
    };

  } // namespace rhi
} // namespace rex
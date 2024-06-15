#include "rex_renderer_core/resources/index_buffer.h"

namespace rex
{
  namespace rhi
  {
    IndexBuffer::IndexBuffer(s32 indexCount, renderer::IndexBufferFormat format)
      : Buffer(indexCount * index_byte_size())
      , m_index_count(indexCount)
      , m_format(format)
    {
    }

    // Return the number of indices in the buffer
    s32 IndexBuffer::count() const
    {
      return m_index_count;
    }
    // Return the size of a single index
    s32 IndexBuffer::index_byte_size() const
    {
      switch (m_format)
      {
      case rex::renderer::IndexBufferFormat::Uint16: return 2;
      case rex::renderer::IndexBufferFormat::Uint32: return 4;
      }

      return -1;
    }
    // Return the total size of the buffer
    s32 IndexBuffer::total_size() const
    {
      return count() * index_byte_size();
    }
    // Return the format of an index in the buffer
    renderer::IndexBufferFormat IndexBuffer::format() const
    {
      return m_format;
    }
  }
}
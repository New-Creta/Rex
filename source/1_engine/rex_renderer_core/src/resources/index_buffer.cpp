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

    s32 IndexBuffer::count() const
    {
      return m_index_count;
    }
    s32 IndexBuffer::index_byte_size() const
    {
      switch (m_format)
      {
      case rex::renderer::IndexBufferFormat::Uint16: return 2;
      case rex::renderer::IndexBufferFormat::Uint32: return 4;
      }

      return -1;
    }
    s32 IndexBuffer::total_size() const
    {
      return count() * index_byte_size();
    }
    renderer::IndexBufferFormat IndexBuffer::format() const
    {
      return m_format;
    }
  }
}
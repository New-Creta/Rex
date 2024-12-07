#include "rex_engine/gfx/resources/index_buffer.h"

namespace rex
{
  namespace gfx
  {
    IndexBuffer::IndexBuffer(s32 indexCount, IndexBufferFormat format)
      : Buffer(indexCount * index_format_size(format))
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
      return index_format_size(m_format);
    }
    // Return the total size of the buffer
    s32 IndexBuffer::total_size() const
    {
      return count() * index_byte_size();
    }
    // Return the format of an index in the buffer
    IndexBufferFormat IndexBuffer::format() const
    {
      return m_format;
    }
  }
}
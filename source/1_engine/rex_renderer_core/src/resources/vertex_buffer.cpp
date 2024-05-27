#include "rex_renderer_core/resources/vertex_buffer.h"

namespace rex
{
  namespace rhi
  {
    VertexBuffer::VertexBuffer(s32 numVertices, rsl::memory_size vertexSize)
      : Buffer(numVertices * vertexSize)
      , m_num_vertices(numVertices)
      , m_vertex_size(vertexSize)
    {}

    s32 VertexBuffer::count() const
    {
      return m_num_vertices;
    }
    rsl::memory_size VertexBuffer::vertex_size() const
    {
      return m_vertex_size;
    }
    rsl::memory_size VertexBuffer::total_size() const
    {
      return m_vertex_size * static_cast<s64>(m_num_vertices);
    }

  }
}
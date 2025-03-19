#include "rex_engine/gfx/resources/vertex_buffer.h"

#include "rex_engine/engine/casting.h"

namespace rex
{
  namespace gfx
  {
    VertexBuffer::VertexBuffer(s32 numVertices, rsl::memory_size vertexSize)
      : Buffer(numVertices * vertexSize)
      , m_num_vertices(numVertices)
      , m_vertex_size(narrow_cast<s32>(vertexSize.size_in_bytes()))
    {}

    s32 VertexBuffer::count() const
    {
      return m_num_vertices;
    }
    rsl::memory_size VertexBuffer::vertex_size() const
    {
      return m_vertex_size;
    }

  }
}
#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/blob_view.h"
#include "rex_renderer_core/resources/input_layout.h"
#include "rex_renderer_core/resources/buffer.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
  namespace rhi
  {
    // A class representing a vertex buffer
    // the api specific buffer will be implemented in a derived class
    class VertexBuffer : public Buffer
    {
    public:
      VertexBuffer(s32 numVertices, rsl::memory_size vertexSize);

      // The number of vertices in the buffer
      s32 count() const;
      // The size of a single vertex
      rsl::memory_size vertex_size() const;

    private:
      s32 m_num_vertices;
      s32 m_vertex_size;
    };

  } // namespace rhi
} // namespace rex
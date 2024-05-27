#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/blob_view.h"
#include "rex_renderer_core/resources/input_layout.h"
#include "rex_renderer_core/resources/buffer.h"

namespace rex
{
  namespace rhi
  {
    class VertexBuffer : public Buffer
    {
    public:
      VertexBuffer(s32 numVertices, rsl::memory_size vertexSize);

      s32 count() const;
      rsl::memory_size vertex_size() const;
      rsl::memory_size total_size() const;

    private:
      s32 m_num_vertices;
      s32 m_vertex_size;
    };

  } // namespace rhi
} // namespace rex
#include "rex_renderer_core/primitives/quad.h"

#include "rex_renderer_core/gfx/vertex.h"

#include "rex_renderer_core/gfx/rhi.h"

namespace rex
{
  namespace gfx
  {
    namespace mesh_factory
    {
      StaticMesh create_quad(f32 w, f32 h)
      {
        rsl::vector<VertexPosNormCol> vertices(4_size);

        // Position coordinates specified in NDC space.
        vertices[0] = VertexPosNormCol(
          glm::vec3(0, -h, 0),
          glm::vec3(0.0f, 0.0f, -1.0f),
          glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        vertices[1] = VertexPosNormCol(
          glm::vec3(0, 0, 0),
          glm::vec3(0.0f, 0.0f, -1.0f),
          glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        vertices[2] = VertexPosNormCol(
          glm::vec3(w, 0, 0),
          glm::vec3(0.0f, 0.0f, -1.0f),
          glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        vertices[3] = VertexPosNormCol(
          glm::vec3(w, -h, 0),
          glm::vec3(0.0f, 0.0f, -1.0f),
          glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        rsl::vector<u16> indices(6_size);
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;

        indices[3] = 0;
        indices[4] = 2;
        indices[5] = 3;

        rsl::unique_ptr<VertexBuffer> vb = rhi::create_vertex_buffer(vertices.size(), sizeof(vertices[0]), vertices.data());
        rsl::unique_ptr<IndexBuffer> ib = rhi::create_index_buffer(indices.size(), IndexBufferFormat::Uint16, indices.data());

        return StaticMesh(rsl::move(vb), rsl::move(ib));
      }
    }
  }
}
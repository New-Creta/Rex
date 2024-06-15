#include "rex_renderer_core/primitives/quad.h"

#include "rex_renderer_core/gfx/vertex.h"

namespace rex
{
  namespace gfx
  {
    namespace mesh_factory
    {
      MeshData16 create_quad(f32 x, f32 y, f32 w, f32 h, f32 depth)
      {
        MeshData16 mesh_data;

        rsl::vector<VertexPosNormCol> vertices(4_size);

        // Position coordinates specified in NDC space.
        vertices[0] = VertexPosNormCol(
          glm::vec3(x, y - h, depth),
          glm::vec3(0.0f, 0.0f, -1.0f),
          glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        vertices[1] = VertexPosNormCol(
          glm::vec3(x, y, depth),
          glm::vec3(0.0f, 0.0f, -1.0f),
          glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        vertices[2] = VertexPosNormCol(
          glm::vec3(x + w, y, depth),
          glm::vec3(0.0f, 0.0f, -1.0f),
          glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        vertices[3] = VertexPosNormCol(
          glm::vec3(x + w, y - h, depth),
          glm::vec3(0.0f, 0.0f, -1.0f),
          glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        rsl::vector<u16> indices(6_size);
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;

        indices[3] = 0;
        indices[4] = 2;
        indices[5] = 3;

        return mesh_data;
      }
    }
  }
}
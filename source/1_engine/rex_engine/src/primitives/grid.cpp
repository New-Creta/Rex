#include "rex_engine/primitives/grid.h"

namespace rex
{
  namespace legacy
  {
    namespace mesh_factory
    {
      MeshData16 create_grid(f32 width, f32 depth, u16 m, u16 n)
      {
        MeshData16 mesh_data;

        const u32 vertex_count = m * n;
        const u32 face_count   = (m - 1) * (n - 1) * 2;

        //
        // Create the vertices.
        //

        const f32 half_width = 0.5f * width;
        const f32 half_depth = 0.5f * depth;

        const f32 dx = width / (n - 1);
        const f32 dz = depth / (m - 1);

        const f32 du = 1.0f / (n - 1);
        const f32 dv = 1.0f / (m - 1);

        mesh_data.reserve_vertices(vertex_count);
        for(u16 i = 0; i < m; ++i)
        {
          const f32 z = half_depth - i * dz;
          for(u16 j = 0; j < n; ++j)
          {
            const f32 x = -half_width + j * dx;

            const glm::vec3 position = glm::vec3(x, 0.0f, z);
            const glm::vec3 normal   = glm::vec3(0.0f, 1.0f, 0.0f);
            const glm::vec3 tangent  = glm::vec3(1.0f, 0.0f, 0.0f);
            const glm::vec2 uv       = glm::vec2(j * du, i * dv);

            mesh_data.insert_vertex(i * n + j, position, normal, tangent, uv);
          }
        }

        //
        // Create the indices.
        //

        mesh_data.reserve_indices(face_count * 3); // 3 indices per face

        // Iterate over each quad and compute indices.
        u16 k = 0;
        for(u16 i = 0; i < m - 1; ++i)
        {
          for(u16 j = 0; j < n - 1; ++j)
          {
            mesh_data.insert_index(k + 0, i * n + j);
            mesh_data.insert_index(k + 1, i * n + j + 1);
            mesh_data.insert_index(k + 2, (i + 1) * n + j);

            mesh_data.insert_index(k + 3, (i + 1) * n + j);
            mesh_data.insert_index(k + 4, i * n + j + 1);
            mesh_data.insert_index(k + 5, (i + 1) * n + j + 1);

            k += 6; // next quad
          }
        }

        return mesh_data;
      }
    } // namespace mesh_factory
  }   // namespace legacy
} // namespace rex
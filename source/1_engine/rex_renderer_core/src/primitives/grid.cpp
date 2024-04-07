#include "rex_renderer_core/primitives/grid.h"

namespace rex
{
  namespace mesh_factory
  {
    MeshData16 create_grid(f32 width, f32 depth, s16 m, s16 n)
    {
      MeshData16 mesh_data;

      const s32 vertex_count = m * n;
      const s32 face_count   = (m - 1) * (n - 1) * 2;

      //
      // Create the vertices.
      //

      const f32 half_width = 0.5f * width;
      const f32 half_depth = 0.5f * depth;

      const f32 dx = width / static_cast<f32>(n - 1);
      const f32 dz = depth / static_cast<f32>(m - 1);

      const f32 du = 1.0f / static_cast<f32>(n - 1);
      const f32 dv = 1.0f / static_cast<f32>(m - 1);

      mesh_data.reserve_vertices(vertex_count);
      for(u16 i = 0; i < m; ++i)
      {
        const f32 z = half_depth - static_cast<f32>(i) * dz;
        for(u16 j = 0; j < n; ++j)
        {
          const f32 x = -half_width + static_cast<f32>(j) * dx;

          glm::vec3 const position = glm::vec3(x, 0.0f, z);
          glm::vec3 const normal   = glm::vec3(0.0f, 1.0f, 0.0f);
          glm::vec4 const col      = glm::vec4(normal, 0.0f);

          mesh_data.insert_vertex(i * n + j, position, normal, col);
        }
      }

      //
      // Create the indices.
      //
      const s32 num_indices = face_count * 3;
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
} // namespace rex
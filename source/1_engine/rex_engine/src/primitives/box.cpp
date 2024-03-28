#include "rex_engine/primitives/box.h"

namespace rex
{
  namespace mesh_factory
  {
    MeshData16 create_box(f32 width, f32 height, f32 depth, u32 subdivisions)
    {
      //
      // Create the vertices.
      //

      const f32 w2 = 0.5f * width;
      const f32 h2 = 0.5f * height;
      const f32 d2 = 0.5f * depth;

      rsl::array<Vertex, 24> v;
      v[0] = Vertex(-w2,  -h2, -d2, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
      v[1] = Vertex(-w2,   h2, -d2, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
      v[2] = Vertex( w2,   h2, -d2, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
      v[3] = Vertex( w2,  -h2, -d2, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
      v[4] = Vertex(-w2,  -h2,  d2, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f);
      v[5] = Vertex(-w2,   h2,  d2, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f);
      v[6] = Vertex( w2,   h2,  d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f);
      v[7] = Vertex( w2,  -h2,  d2, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f);

      rsl::array<u16, 36> i =
      {
          0, 1, 2, 0, 2, 3,
          4, 6, 5, 4, 7, 6,
          4, 5, 1, 4, 1, 0,
          3, 2, 6, 3, 6, 7,
          1, 5, 6, 1, 6, 2,
          4, 0, 3, 4, 3, 7
      };

      MeshData16 mesh_data;
      mesh_data.assign_vertices(v.data(), v.size());
      mesh_data.assign_indices(i.data(), i.size());

      return mesh_data;
    }
  } // namespace mesh_factory
} // namespace rex
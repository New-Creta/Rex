#include "rex_engine/primitives/box.h"

namespace rex
{
  namespace mesh_factory
  {
    MeshData16 create_box(f32 width, f32 height, f32 depth, u32 subdivisions)
    {
      MeshData16 mesh_data;

      //
      // Create the vertices.
      //

      rsl::array<Vertex, 24> v;

      const f32 w2 = 0.5f * width;
      const f32 h2 = 0.5f * height;
      const f32 d2 = 0.5f * depth;

      // Fill in the front face vertex data.
      v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
      v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
      v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
      v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

      // Fill in the back face vertex data.
      v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
      v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
      v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
      v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

      // Fill in the top face vertex data.
      v[8]  = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
      v[9]  = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
      v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
      v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

      // Fill in the bottom face vertex data.
      v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
      v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
      v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
      v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

      // Fill in the left face vertex data.
      v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
      v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
      v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
      v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

      // Fill in the right face vertex data.
      v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
      v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
      v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
      v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

      mesh_data.assign_vertices(v.data(), v.size());

      //
      // Create the indices.
      //

      rsl::array<u16, 36> i;

      // Fill in the front face index data
      i[0] = 0;
      i[1] = 1;
      i[2] = 2;
      i[3] = 0;
      i[4] = 2;
      i[5] = 3;

      // Fill in the back face index data
      i[6]  = 4;
      i[7]  = 5;
      i[8]  = 6;
      i[9]  = 4;
      i[10] = 6;
      i[11] = 7;

      // Fill in the top face index data
      i[12] = 8;
      i[13] = 9;
      i[14] = 10;
      i[15] = 8;
      i[16] = 10;
      i[17] = 11;

      // Fill in the bottom face index data
      i[18] = 12;
      i[19] = 13;
      i[20] = 14;
      i[21] = 12;
      i[22] = 14;
      i[23] = 15;

      // Fill in the left face index data
      i[24] = 16;
      i[25] = 17;
      i[26] = 18;
      i[27] = 16;
      i[28] = 18;
      i[29] = 19;

      // Fill in the right face index data
      i[30] = 20;
      i[31] = 21;
      i[32] = 22;
      i[33] = 20;
      i[34] = 22;
      i[35] = 23;

      mesh_data.assign_indices(i.data(), i.size());

      for(uint32 s = 0; s < subdivisions; ++s)
      {
        subdivide<u16>(mesh_data);
      }

      return mesh_data;
    }
  } // namespace mesh_factory
} // namespace rex
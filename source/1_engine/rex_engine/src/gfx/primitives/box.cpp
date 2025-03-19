#include "rex_engine/gfx/primitives/box.h"

#include "rex_engine/gfx/system/gal.h"

namespace rex
{
  namespace gfx
  {
    namespace mesh_factory
    {
      StaticMesh create_box(f32 width, f32 height, f32 depth)
      {
        //
        // Create the vertices.
        //

        const f32 w2 = 0.5f * width;
        const f32 h2 = 0.5f * height;
        const f32 d2 = 0.5f * depth;

        rsl::array<VertexPosNormTex, 24> v;

        // Front Face
        v[0] = VertexPosNormTex(glm::vec3(-w2, -h2, d2), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f));
        v[1] = VertexPosNormTex(glm::vec3( w2, -h2, d2), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
        v[2] = VertexPosNormTex(glm::vec3( w2,  h2, d2), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f));
        v[3] = VertexPosNormTex(glm::vec3(-w2,  h2, d2), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f));

        // Back Face
        v[4] = VertexPosNormTex(glm::vec3( w2, -h2, -d2), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f));
        v[5] = VertexPosNormTex(glm::vec3(-w2, -h2, -d2), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f));
        v[6] = VertexPosNormTex(glm::vec3(-w2,  h2, -d2), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f));
        v[7] = VertexPosNormTex(glm::vec3( w2,  h2, -d2), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f));

        // Left Face
        v[8] = VertexPosNormTex(glm::vec3( -w2, -h2, -d2), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        v[9] = VertexPosNormTex(glm::vec3( -w2, -h2,  d2), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        v[10] = VertexPosNormTex(glm::vec3(-w2,  h2,  d2), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
        v[11] = VertexPosNormTex(glm::vec3(-w2,  h2, -d2), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));

        // Right Face
        v[12] = VertexPosNormTex(glm::vec3(w2, -h2,  d2), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        v[13] = VertexPosNormTex(glm::vec3(w2, -h2, -d2), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        v[14] = VertexPosNormTex(glm::vec3(w2,  h2, -d2), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
        v[15] = VertexPosNormTex(glm::vec3(w2,  h2,  d2), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f));

        // Top Face
        v[16] = VertexPosNormTex(glm::vec3(-w2, h2,  d2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        v[17] = VertexPosNormTex(glm::vec3( w2, h2,  d2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        v[18] = VertexPosNormTex(glm::vec3( w2, h2, -d2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
        v[19] = VertexPosNormTex(glm::vec3(-w2, h2, -d2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f));

        // Bottom Face
        v[20] = VertexPosNormTex(glm::vec3(-w2, -h2, -d2), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
        v[21] = VertexPosNormTex(glm::vec3( w2, -h2, -d2), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f));
        v[22] = VertexPosNormTex(glm::vec3( w2, -h2,  d2), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
        v[23] = VertexPosNormTex(glm::vec3(-w2, -h2,  d2), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f));


        rsl::array<u16, 36> i =
        {
          // Front Face
          0, 1, 2,  // First Triangle
          2, 3, 0,  // Second Triangle
          
          // Back Face
          4, 5, 6,  // First Triangle
          6, 7, 4,  // Second Triangle
          
          // Left Face
          8, 9, 10, // First Triangle
          10, 11, 8, // Second Triangle
          
          // Right Face
          12, 13, 14, // First Triangle
          14, 15, 12, // Second Triangle
          
          // Top Face
          16, 17, 18, // First Triangle
          18, 19, 16, // Second Triangle
          
          // Bottom Face
          20, 21, 22, // First Triangle
          22, 23, 20  // Second Triangle
        };

        rsl::unique_ptr<VertexBuffer> vb = gal()->create_vertex_buffer(v.size(), sizeof(v[0]), v.data());
        rsl::unique_ptr<IndexBuffer> ib = gal()->create_index_buffer(i.size(), IndexBufferFormat::Uint16, i.data());

        return StaticMesh(rsl::move(vb), rsl::move(ib));
      }

      StaticMesh create_cube(f32 size)
      {
        return create_box(size, size, size);
      }

    } // namespace mesh_factory
  }
} // namespace rex
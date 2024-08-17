#include "rex_renderer_core/primitives/cylinder.h"
#include <glm/gtc/constants.hpp>

#include "rex_renderer_core/gfx/rhi.h"

namespace rex
{
  namespace gfx
  {
    namespace mesh_factory
    {
      namespace internal
      {
        void build_cylinder_top_cap(f32 topRadius, f32 height, s32 sliceCount, MeshData16& meshData)
        {
          //
          // Build top cap.
          //

          const s32 base_index = meshData.vertices().size();

          const f32 y = 0.5f * height;
          const f32 d_theta = 2.0f * glm::pi<f32>() / static_cast<f32>(sliceCount);

          // Duplicate cap ring vertices because the texture coordinates and normals differ.
          for (s32 i = 0; i <= sliceCount; ++i)
          {
            const f32 x = topRadius * cosf(static_cast<f32>(i) * d_theta);
            const f32 z = topRadius * sinf(static_cast<f32>(i) * d_theta);

            // Scale down by the height to try and make top cap texture coord area
            // proportional to base.
            const f32 u = x / height + 0.5f;
            const f32 v = z / height + 0.5f;

            meshData.add_vertex(VertexPosNormTex(glm::vec3(x, y, z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
          }

          // Cap center VertexPosNormTex.
          meshData.add_vertex(VertexPosNormTex(glm::vec3(0.0f, y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));

          // Index of center VertexPosNormTex.
          const u16 center_index = static_cast<u16>(meshData.vertices().size()) - 1;

          for (s32 i = 0; i < sliceCount; ++i)
          {
            meshData.add_index(center_index);
            meshData.add_index(static_cast<u16>(base_index + i + 1));
            meshData.add_index(static_cast<u16>(base_index + i));
          }
        }

        void build_cylinder_bottom_cap(f32 bottomRadius, f32 height, s32 sliceCount, MeshData16& meshData)
        {
          //
          // Build bottom cap.
          //

          const u16 base_index = static_cast<u16>(meshData.vertices().size());
          const f32 y = -0.5f * height;

          // vertices of ring
          const f32 d_theta = 2.0f * glm::pi<f32>() / static_cast<f32>(sliceCount);
          for (s32 i = 0; i <= sliceCount; ++i)
          {
            const f32 x = bottomRadius * cosf(static_cast<f32>(i) * d_theta);
            const f32 z = bottomRadius * sinf(static_cast<f32>(i) * d_theta);

            // Scale down by the height to try and make top cap texture coord area
            // proportional to base.
            const f32 u = x / height + 0.5f;
            const f32 v = z / height + 0.5f;

            meshData.add_vertex(VertexPosNormTex(glm::vec3(x, y, z), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
          }

          // Cap center VertexPosNormTex.
          meshData.add_vertex(VertexPosNormTex(glm::vec3(0.0f, y, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));

          // Cache the index of center VertexPosNormTex.
          const u16 center_index = static_cast<u16>(meshData.vertices().size()) - 1;

          for (s32 i = 0; i < sliceCount; ++i)
          {
            meshData.add_index(center_index);
            meshData.add_index(static_cast<u16>(base_index + i));
            meshData.add_index(static_cast<u16>(base_index + i + 1));
          }
        }
      } // namespace internal

      StaticMesh create_cylinder(f32 bottomRadius, f32 topRadius, f32 height, u16 sliceCount, u16 stackCount)
      {
        MeshData16 mesh_data;

        //
        // Build Stacks.
        //

        const f32 stack_height = height / static_cast<f32>(stackCount);

        // Amount to increment radius as we move up each stack level from bottom to top.
        const f32 radius_step = (topRadius - bottomRadius) / static_cast<f32>(stackCount);

        const u16 ring_count = stackCount + 1;

        // Compute vertices for each stack ring starting at the bottom and moving up.
        for (u16 i = 0; i < ring_count; ++i)
        {
          const f32 y = -0.5f * height + static_cast<f32>(i) * stack_height;
          const f32 r = bottomRadius + static_cast<f32>(i) * radius_step;

          // vertices of ring
          const f32 d_theta = 2.0f * glm::pi<f32>() / static_cast<f32>(sliceCount);
          for (u16 j = 0; j <= sliceCount; ++j)
          {
            const f32 c = cosf(static_cast<f32>(j) * d_theta);
            const f32 s = sinf(static_cast<f32>(j) * d_theta);

            const glm::vec3 position = glm::vec3(r * c, y, r * s);

            // Cylinder can be parameterized as follows, where we introduce v
            // parameter that goes in the same direction as the v tex-coord
            // so that the bitangent goes in the same direction as the v tex-coord.
            //   Let r0 be the bottom radius and let r1 be the top radius.
            //   y(v) = h - hv for v in [0,1].
            //   r(v) = r1 + (r0-r1)v
            //
            //   x(t, v) = r(v)*cos(t)
            //   y(t, v) = h - hv
            //   z(t, v) = r(v)*sin(t)
            //
            //  dx/dt = -r(v)*sin(t)
            //  dy/dt = 0
            //  dz/dt = +r(v)*cos(t)
            //
            //  dx/dv = (r0-r1)*cos(t)
            //  dy/dv = -h
            //  dz/dv = (r0-r1)*sin(t)

            // This is unit length.
            const glm::vec3 tangent = glm::vec3(-s, 0.0f, c);

            const f32 dr = bottomRadius - topRadius;
            const glm::vec3 bitangent(dr * c, -height, dr * s);

            const glm::vec3 t = tangent;
            const glm::vec3 b = bitangent;
            const glm::vec3 n = glm::normalize(glm::cross(t, b));
            const glm::vec3 normal = n;
            const glm::vec4 col(normal, 1.0f);

            mesh_data.add_vertex(VertexPosNormTex(position, normal, col));
          }
        }

        // Add one because we duplicate the first and last VertexPosNormTex per ring
        // since the texture coordinates are different.
        const s32 ring_vertex_count = static_cast<s32>(sliceCount) + 1;

        // Compute indices for each stack.
        for (s32 i = 0; i < stackCount; ++i)
        {
          for (s32 j = 0; j < sliceCount; ++j)
          {
            mesh_data.add_index(static_cast<u16>(i * ring_vertex_count + j));
            mesh_data.add_index(static_cast<u16>((i + 1) * ring_vertex_count + j));
            mesh_data.add_index(static_cast<u16>((i + 1) * ring_vertex_count + j + 1));

            mesh_data.add_index(static_cast<u16>(i * ring_vertex_count + j));
            mesh_data.add_index(static_cast<u16>((i + 1) * ring_vertex_count + j + 1));
            mesh_data.add_index(static_cast<u16>(i * ring_vertex_count + j + 1));
          }
        }

        rsl::unique_ptr<VertexBuffer> vb = rhi::create_vertex_buffer(mesh_data.vertices().size(), mesh_data.vertex_size(), mesh_data.vertices().data());
        rsl::unique_ptr<IndexBuffer> ib = rhi::create_index_buffer(mesh_data.indices().size(), IndexBufferFormat::Uint16, mesh_data.indices().data());

        return StaticMesh(rsl::move(vb), rsl::move(ib));
      }

    } // namespace mesh_factory
  }
} // namespace rex
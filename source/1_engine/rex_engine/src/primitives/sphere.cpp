#include "rex_engine/primitives/sphere.h"

namespace rex
{
  namespace mesh_factory
  {
    MeshData16 create_sphere(f32 radius, u16 sliceCount, u16 stackCount)
    {
      MeshData16 mesh_data;

      //
      // Compute the vertices stating at the top pole and moving down the stacks.
      //

      // Poles: note that there will be texture coordinate distortion as there is
      // not a unique point on the texture map to assign to the pole when mapping
      // a rectangular texture onto a sphere.
      const Vertex top_vertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
      const Vertex bottom_vertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

      mesh_data.add_vertex(top_vertex);

      const f32 phi_step   = glm::pi<f32>() / stackCount;
      const f32 theta_step = 2.0f * glm::pi<f32>() / sliceCount;

      // Compute vertices for each stack ring (do not count the poles as rings).
      for(u16 i = 1; i <= stackCount - 1; ++i)
      {
        const f32 phi = i * phi_step;

        // Vertices of ring.
        for(u16 j = 0; j <= sliceCount; ++j)
        {
          const f32 theta = j * theta_step;

          Vertex v;

          // spherical to cartesian
          v.position.x = radius * sinf(phi) * cosf(theta);
          v.position.y = radius * cosf(phi);
          v.position.z = radius * sinf(phi) * sinf(theta);

          // Partial derivative of P with respect to theta
          v.tangent.x = -radius * sinf(phi) * sinf(theta);
          v.tangent.y = 0.0f;
          v.tangent.z = +radius * sinf(phi) * cosf(theta);

          const glm::vec3 tangent = v.tangent;
          v.tangent               = glm::normalize(tangent);

          const glm::vec3 p = v.position;
          v.normal          = glm::normalize(p);

          v.uv.x = theta / glm::two_pi<f32>();
          v.uv.y = phi / glm::pi<f32>();

          mesh_data.add_vertex(v);
        }
      }

      mesh_data.add_vertex(bottom_vertex);

      //
      // Compute indices for top stack.  u16he top stack was written first to the vertex buffer
      // and connects the top pole to the first ring.
      //

      for(u16 i = 1; i <= sliceCount; ++i)
      {
        mesh_data.add_index(0);
        mesh_data.add_index(i + 1);
        mesh_data.add_index(i);
      }

      //
      // Compute indices for inner stacks (not connected to poles).
      //

      // Offset the indices to the index of the first vertex in the first ring.
      // u16his is just skipping the top pole vertex.
      u16 base_index              = 1;
      const u16 ring_vertex_count = sliceCount + 1;
      for(u16 i = 0; i < stackCount - 2; ++i)
      {
        for(u16 j = 0; j < sliceCount; ++j)
        {
          mesh_data.add_index(base_index + i * ring_vertex_count + j);
          mesh_data.add_index(base_index + i * ring_vertex_count + j + 1);
          mesh_data.add_index(base_index + (i + 1) * ring_vertex_count + j);

          mesh_data.add_index(base_index + (i + 1) * ring_vertex_count + j);
          mesh_data.add_index(base_index + i * ring_vertex_count + j + 1);
          mesh_data.add_index(base_index + (i + 1) * ring_vertex_count + j + 1);
        }
      }

      //
      // Compute indices for bottom stack.  u16he bottom stack was written last to the vertex buffer
      // and connects the bottom pole to the bottom ring.
      //

      // South pole vertex was added last.
      const u16 south_pole_index = static_cast<u16>(mesh_data.vertices().size()) - 1;

      // Offset the indices to the index of the first vertex in the last ring.
      base_index = south_pole_index - ring_vertex_count;

      for(u16 i = 0; i < sliceCount; ++i)
      {
        mesh_data.add_index(south_pole_index);
        mesh_data.add_index(base_index + i);
        mesh_data.add_index(base_index + i + 1);
      }

      return mesh_data;
    }

    template <typename U16>
    MeshData<U16> create_geo_sphere(f32 radius, u32 numSubdivisions)
    {
      MeshData<u16> mesh_data;

      // Approximate a sphere by tessellating an icosahedron.

      const f32 X = 0.525731f;
      const f32 Z = 0.850651f;

      glm::vec3 pos[12] = {glm::vec3(-X, 0.0f, Z), glm::vec3(X, 0.0f, Z),   glm::vec3(-X, 0.0f, -Z), glm::vec3(X, 0.0f, -Z), glm::vec3(0.0f, Z, X),  glm::vec3(0.0f, Z, -X),
                           glm::vec3(0.0f, -Z, X), glm::vec3(0.0f, -Z, -X), glm::vec3(Z, X, 0.0f),   glm::vec3(-Z, X, 0.0f), glm::vec3(Z, -X, 0.0f), glm::vec3(-Z, -X, 0.0f)};

      u16 k[60] = {1, 4, 0, 4, 9, 0, 4, 5, 9, 8, 5, 4, 1, 8, 4, 1, 10, 8, 10, 3, 8, 8, 3, 5, 3, 2, 5, 3, 7, 2, 3, 10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6, 1, 0, 10, 1, 6, 11, 0, 9, 2, 11, 9, 5, 2, 9, 11, 2, 7};

      mesh_data.resize_vertices(12);
      mesh_data.assign_indices(k, 60);

      for(u32 i = 0; i < 12; ++i)
      {
        mesh_data.vertices()[i].position = pos[i];
      }

      for(u32 i = 0; i < numSubdivisions; ++i)
      {
        subdivide<u16>(mesh_data);
      }

      // Project vertices onto sphere and scale.
      for(u32 i = 0; i < mesh_data.vertices().size(); ++i)
      {
        // Project onto unit sphere.
        glm::vec3 n = glm::normalize(&mesh_data.vertices()[i].position);

        // Project onto sphere.
        glm::vec3 p = radius * n;

        mesh_data.vertices()[i].position = p;
        mesh_data.vertices()[i].normal   = n;

        // Derive texture coordinates from spherical coordinates.
        f32 theta = atan2f(mesh_data.vertices()[i].position.z, mesh_data.vertices()[i].position.x);

        // Put in [0, 2pi].
        if(theta < 0.0f)
        {
          theta += glm::two_pi<f32>();
        }

        f32 phi = acosf(mesh_data.vertices()[i].position.y / radius);

        mesh_data.vertices()[i].uv.x = theta / glm::two_pi<f32>();
        mesh_data.vertices()[i].uv.y = phi / glm::pi<f32>();

        // Partial derivative of P with respect to theta
        mesh_data.vertices()[i].tangent.x = -radius * sinf(phi) * sinf(theta);
        mesh_data.vertices()[i].tangent.y = 0.0f;
        mesh_data.vertices()[i].tangent.z = +radius * sinf(phi) * cosf(theta);

        glm::vec3 u16                   = mesh_data.vertices()[i].tangent;
        mesh_data.vertices()[i].tangent = glm::normalize(u16);
      }

      return mesh_data;
    }
  } // namespace mesh_factory
} // namespace rex
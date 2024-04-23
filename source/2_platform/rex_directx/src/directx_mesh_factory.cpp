#include "rex_renderer_core/mesh_factory.h"
#include "rex_renderer_core/renderer.h"

#include "rex_directx/scenegraph/mesh_node.h"
#include "rex_directx/utility/vertex_types.h"

#include "rex_directx/mesh.h"

#include "rex_std/vector.h"

#include <DirectXMath.h>

namespace rex
{
    namespace mesh_factory
    {
        using VertexCollection = rsl::vector<renderer::XMVertexPosCol>;
        using IndexCollection = rsl::vector<uint16_t>;

        namespace internal
        {
            rsl::shared_ptr<SceneNode> create_scene(const VertexCollection& vertices, const IndexCollection& indices)
            {
                if (vertices.empty())
                {
                    return nullptr;
                }

                auto new_vertex_buffer = renderer::copy_vertex_buffer(vertices);
                auto new_index_Buffer  = renderer::copy_index_buffer(indices);

                auto new_mesh = rsl::make_shared<renderer::Mesh>();

                new_mesh->set_vertex_buffer(0, new_vertex_buffer, vertices.size());
                new_mesh->set_index_buffer(new_index_Buffer, indices.size());

                auto new_node = rsl::make_shared<MeshNode>();
                new_node->add_mesh(new_mesh);

                return new_node;
            }

            // Definition for inline functions.
            inline DirectX::XMVECTOR get_circle_vector(size_t i, size_t tessellation) noexcept
            {
                float angle = float(i) * DirectX::XM_2PI / float(tessellation);
                float dx, dz;

                DirectX::XMScalarSinCos(&dx, &dz, angle);

                DirectX::XMVECTORF32 v = { { { dx, 0, dz, 0 } } };
                return v;
            }

            void create_cylinder_cap(VertexCollection& vertices, IndexCollection& indices, size_t tessellation, float height, float radius, bool isTop, DirectX::XMFLOAT3 color)
            {
                // Create cap indices.
                for (size_t i = 0; i < tessellation - 2; i++)
                {
                    u16 i1 = (i + 1u) % (u16)tessellation;
                    u16 i2 = (i + 2u) % (u16)tessellation;

                    if (isTop)
                    {
                        rsl::swap(i1, i2);
                    }

                    u16 vbase = (u16)vertices.size();
                    indices.push_back(vbase + i2);
                    indices.push_back(vbase + i1);
                    indices.push_back(vbase);
                }

                // Which end of the cylinder is this?
                DirectX::XMVECTOR normal = DirectX::g_XMIdentityR1;

                if (!isTop)
                {
                    normal = DirectX::XMVectorNegate(normal);
                }

                // Create cap vertices.
                for (size_t i = 0; i < tessellation; i++)
                {
                    DirectX::XMVECTOR circle_vector = get_circle_vector(i, tessellation);
                    DirectX::XMVECTOR position = DirectX::XMVectorAdd(DirectX::XMVectorScale(circle_vector, radius), DirectX::XMVectorScale(normal, height));
                    vertices.emplace_back(position, DirectX::XMLoadFloat3(&color));
                }
            }
        }

        /**
         * Create a cube.
         *
         * @param size The size of one side of the cube.
         * @param reverseWinding Whether to reverse the winding order of the triangles (useful for skyboxes).
         */
        rsl::shared_ptr<SceneNode> create_cube(const glm::vec3& color, f32 size)
        {
          // Convert to directx type
          DirectX::XMFLOAT3 xm_color;
          DirectX::XMStoreFloat3(&xm_color, {color.x, color.y, color.z});
          
          // Create cube
          //

            // Cube is centered at 0,0,0.
            float s = size * 0.5f;

            // 8 vertex position
            DirectX::XMFLOAT3 p[8] = 
            { 
                { +s, +s, -s }, 
                { +s, +s, +s },
                { +s, -s, +s },
                { +s, -s, -s },
                { -s, +s, +s },
                { -s, +s, -s }, 
                { -s, -s, -s }, 
                { -s, -s, +s }
            };

            // Indices for the vertex positions.
            u16 i[24] = {
                0, 1, 2, 3,  // +X
                4, 5, 6, 7,  // -X
                4, 1, 0, 5,  // +Y
                2, 7, 6, 3,  // -Y
                1, 4, 7, 2,  // +Z
                5, 0, 3, 6   // -Z
            };

            VertexCollection vertices;
            IndexCollection indices;

            for (u16 f = 0; f < 6; ++f)  // For each face of the cube.
            {
                // Four vertices per face.
                vertices.emplace_back(p[static_cast<s32>(i[static_cast<s32>(f * 4 + 0)])], xm_color);
                vertices.emplace_back(p[static_cast<s32>(i[static_cast<s32>(f * 4 + 1)])], xm_color);
                vertices.emplace_back(p[static_cast<s32>(i[static_cast<s32>(f * 4 + 2)])], xm_color);
                vertices.emplace_back(p[static_cast<s32>(i[static_cast<s32>(f * 4 + 3)])], xm_color);

                // First triangle.
                indices.emplace_back(static_cast<u16>(f * 4u + 0u));
                indices.emplace_back(static_cast<u16>(f * 4u + 1u));
                indices.emplace_back(static_cast<u16>(f * 4u + 2u));

                // Second triangle
                indices.emplace_back(static_cast<u16>(f * 4u + 2u));
                indices.emplace_back(static_cast<u16>(f * 4u + 3u));
                indices.emplace_back(static_cast<u16>(f * 4u + 0u));
            }

            return internal::create_scene(vertices, indices);
        }

        /**
         * Create a sphere.
         *
         * @param radius Radius of the sphere.
         * @param tessellation Determines how smooth the sphere is.
         * @param reverseWinding Whether to reverse the winding order of the triangles (useful for sydomes).
         */
        rsl::shared_ptr<SceneNode> create_sphere(const glm::vec3& color, f32 radius, u32 tessellation)
        {
            assert(tessellation > 3 && "tessellation parameter out of range");

            // Convert to directx type
            DirectX::XMFLOAT3 xm_color;
            DirectX::XMStoreFloat3(&xm_color, {color.x, color.y, color.z});

            // Create sphere
            //
            VertexCollection vertices;
            IndexCollection  indices;

            u32 vertical_segments = tessellation;
            u32 horizontal_segments = tessellation * 2u;

            // Create rings of vertices at progressively higher latitudes.
            for (size_t i = 0; i <= vertical_segments; i++)
            {
                float latitude = (i * DirectX::XM_PI / vertical_segments) - DirectX::XM_PIDIV2;
                float dy, dxz;

                DirectX::XMScalarSinCos(&dy, &dxz, latitude);

                // Create a single ring of vertices at this latitude.
                for (size_t j = 0; j <= horizontal_segments; j++)
                {
                    float longitude = j * DirectX::XM_2PI / horizontal_segments;
                    float dx, dz;

                    DirectX::XMScalarSinCos(&dx, &dz, longitude);

                    dx *= dxz;
                    dz *= dxz;

                    auto normal = DirectX::XMVectorSet(dx, dy, dz, 0);
                    auto position = DirectX::operator*(radius, normal);

                    vertices.emplace_back(position, DirectX::XMLoadFloat3(&xm_color));
                }
            }

            // Fill the index buffer with triangles joining each pair of latitude rings.
            u16 stride = (u16)(horizontal_segments + 1u);

            for (u16 i = 0; i < vertical_segments; i++)
            {
              for(u16 j = 0; j <= horizontal_segments; j++)
                {
                    u16 nextI = i + 1;
                    u16 nextJ = (j + 1) % stride;

                    indices.push_back(i * stride + nextJ);
                    indices.push_back(nextI * stride + j);
                    indices.push_back(i * stride + j);

                    indices.push_back(nextI * stride + nextJ);
                    indices.push_back(nextI * stride + j);
                    indices.push_back(i * stride + nextJ);
                }
            }

            return internal::create_scene(vertices, indices);
        }

        /**
         * Create a Cylinder
         *
         * @param radius The radius of the primary axis of the cylinder.
         * @param hight The height of the cylinder.
         * @param tessellation How smooth the cylinder will be.
         * @param reverseWinding Whether to reverse the winding order of the triangles.
         */
        rsl::shared_ptr<SceneNode> create_cylinder(const glm::vec3& color, f32 radius, f32 height, u32 tessellation)
        {
            assert(tessellation > 3 && "tessellation parameter out of range");

            // Convert to directx type
            DirectX::XMFLOAT3 xm_color;
            DirectX::XMStoreFloat3(&xm_color, {color.x, color.y, color.z});

            // Create Cylinder
            //
            VertexCollection vertices;
            IndexCollection  indices;

            height /= 2;

            DirectX::XMVECTOR top_offset = DirectX::XMVectorScale(DirectX::g_XMIdentityR1, height);

            u32 stride = tessellation + 1;

            DirectX::XMVECTOR simd_color = DirectX::XMLoadFloat3(&xm_color);

            // Create a ring of triangles around the outside of the cylinder.
            for (u16 i = 0; i <= tessellation; i++)
            {
                DirectX::XMVECTOR normal = internal::get_circle_vector(i, tessellation);
                DirectX::XMVECTOR side_offset = DirectX::XMVectorScale(normal, radius);

                vertices.emplace_back(DirectX::XMVectorAdd(side_offset, top_offset), simd_color);
                vertices.emplace_back(DirectX::XMVectorSubtract(side_offset, top_offset), simd_color);

                indices.push_back(i * 2u + 1u);
                indices.push_back((i * 2u + 2u) % (stride * 2u));
                indices.push_back(i * 2u);

                indices.push_back((i * 2u + 3u) % (stride * 2u));
                indices.push_back((i * 2u + 2u) % (stride * 2u));
                indices.push_back(i * 2u + 1u);
            }

            // Create flat triangle fan caps to seal the top and bottom.
            internal::create_cylinder_cap(vertices, indices, tessellation, height, radius, true, xm_color);
            internal::create_cylinder_cap(vertices, indices, tessellation, height, radius, false, xm_color);

            return internal::create_scene(vertices, indices);
        }

        /**
         * Create a plane.
         *
         * @param width The width of the plane.
         * @param height The height of the plane.
         * @reverseWinding Whether to reverse the winding order of the plane.
         */
        rsl::shared_ptr<SceneNode> create_plane(const glm::vec3& color, f32 width, f32 height)
        {
          // Convert to directx type
          DirectX::XMFLOAT3 xm_color;
          DirectX::XMStoreFloat3(&xm_color, {color.x, color.y, color.z});

          // Clear plane
          // 
          // clang-format off
            // Define a plane that is aligned with the X-Z plane and the normal is facing up in the Y-axis.
            VertexCollection vertices = 
            {
                renderer::XMVertexPosCol(DirectX::XMFLOAT3(-0.5f * width, 0.0f, 0.5f * height), xm_color),  // 0
                renderer::XMVertexPosCol(DirectX::XMFLOAT3(0.5f * width, 0.0f, 0.5f * height), xm_color),   // 1
                renderer::XMVertexPosCol(DirectX::XMFLOAT3(0.5f * width, 0.0f, -0.5f * height), xm_color),  // 2
                renderer::XMVertexPosCol(DirectX::XMFLOAT3(-0.5f * width, 0.0f, -0.5f * height), xm_color)  // 3
            };
            
            IndexCollection indices = { 1, 3, 0, 2, 3, 1 };

            return internal::create_scene(vertices, indices);
        }
    }
}
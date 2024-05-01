#include "regina/sample_scene.h"

#include "rex_engine/app/core_application.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_renderer_core/primitives/box.h"
#include "rex_renderer_core/primitives/cylinder.h"
#include "rex_renderer_core/primitives/grid.h"
#include "rex_renderer_core/primitives/mesh_factory.h"
#include "rex_renderer_core/primitives/sphere.h"
#include "rex_renderer_core/rendering/depth_info.h"
#include "rex_renderer_core/rendering/vertex.h"
#include "rex_renderer_core/system/renderer.h"
#include "rex_renderer_core/system/rhi.h"
#include "rex_std/bonus/math/color.h"
#include "rex_windows/app/gui_application.h"

#include <glm/gtc/matrix_transform.hpp>

namespace regina
{
  SampleScene::SampleScene()
  {
    build_geometry();

    // input layout should be tied to the render item

    // shader and pso should be part of the material system
  }

  void SampleScene::update_object_constant_buffers()
  {
    // Nothing to implement
  }

  void SampleScene::build_geometry()
  {
    // Create the vertices and indices for all objects we want to render
    rsl::vector<rex::renderer::VertexPosNormCol> vertices;
    rsl::vector<u16> indices;

    const rex::renderer::Submesh box      = build_submesh(rex::mesh_factory::create_box(1.5f, 1.5f, 1.5f), vertices, indices);
    const rex::renderer::Submesh grid     = build_submesh(rex::mesh_factory::create_grid(20.0f, 30.0f, 60, 40), vertices, indices);
    const rex::renderer::Submesh sphere   = build_submesh(rex::mesh_factory::create_sphere(0.5f, 20, 20), vertices, indices);
    const rex::renderer::Submesh cylinder = build_submesh(rex::mesh_factory::create_cylinder(0.5f, 0.3f, 3.0f, 20, 20), vertices, indices);

    // Create the descs for the vertex buffer and index buffer
    m_vb = rex::memory::Blob(rsl::make_unique<rex::renderer::VertexPosNormCol[]>(vertices.size())); // NOLINT(modernize-avoid-c-arrays)
    m_vb.write(vertices.data(), m_vb.size());
    rex::rhi::VertexBufferDesc vb_desc {rex::memory::BlobView(m_vb), sizeof(rex::renderer::VertexPosNormCol)/*, rex::renderer::VertexPosCol::layout()*/};

    m_ib = rex::memory::Blob(rsl::make_unique<u16[]>(indices.size())); // NOLINT(modernize-avoid-c-arrays)
    m_ib.write(indices.data(), m_ib.size());
    rex::rhi::IndexBufferDesc ib_desc {rex::memory::BlobView(m_ib), rex::renderer::IndexBufferFormat::Uint16, indices.size()};

    // Create the mesh geometry
    m_geometry = rsl::make_unique<rex::renderer::Mesh>("scene_geometry"_med, vb_desc, ib_desc);

    m_geometry->add_submesh("box"_small, box);
    m_geometry->add_submesh("grid"_small, grid);
    m_geometry->add_submesh("sphere"_small, sphere);
    m_geometry->add_submesh("cylinder"_small, cylinder);

    // Box
    glm::mat4 world = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    world           = glm::transpose(world); // Always transpose as we're working with Direct X
    rex::memory::Blob box_cb(rsl::make_unique<rsl::byte[]>(sizeof(world))); // NOLINT(modernize-avoid-c-arrays)
    box_cb.write(&world, rsl::memory_size(sizeof(world)));
    rex::renderer::add_mesh(m_geometry.get(), box, rex::memory::BlobView(box_cb));

    // Grid
    glm::mat4 grid_trans = glm::mat4(1.0f);
    grid_trans           = glm::transpose(grid_trans); // Always transpose as we're working with Direct X
    rex::memory::Blob grid_cb(rsl::make_unique<rsl::byte[]>(sizeof(grid_trans))); // NOLINT(modernize-avoid-c-arrays)
    grid_cb.write(&grid_trans, rsl::memory_size(sizeof(grid_trans)));
    rex::renderer::add_mesh(m_geometry.get(), grid, rex::memory::BlobView(grid_cb));

    // Spheres and Cylinders
    for(s32 i = 0; i < 5; ++i)
    {
      glm::mat4 left_cyl_world     = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 1.5f, -10.0f + static_cast<f32>(i) * 5.0f));
      left_cyl_world               = glm::transpose(left_cyl_world);     // Always transpose as we're working with Direct X
      glm::mat4 right_cyl_world    = glm::translate(glm::mat4(1.0f), glm::vec3(+5.0f, 1.5f, -10.0f + static_cast<f32>(i) * 5.0f));
      right_cyl_world              = glm::transpose(right_cyl_world);    // Always transpose as we're working with Direct X
      glm::mat4 left_sphere_world  = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 3.5f, -10.0f + static_cast<f32>(i) * 5.0f));
      left_sphere_world            = glm::transpose(left_sphere_world);  // Always transpose as we're working with Direct X
      glm::mat4 right_sphere_world = glm::translate(glm::mat4(1.0f), glm::vec3(+5.0f, 3.5f, -10.0f + static_cast<f32>(i) * 5.0f));
      right_sphere_world           = glm::transpose(right_sphere_world); // Always transpose as we're working with Direct X

      rex::memory::Blob left_cyl_cb(rsl::make_unique<rsl::byte[]>(sizeof(glm::mat4))); // NOLINT(modernize-avoid-c-arrays)
      rex::memory::Blob right_cyl_cb(rsl::make_unique<rsl::byte[]>(sizeof(glm::mat4))); // NOLINT(modernize-avoid-c-arrays)
      rex::memory::Blob left_sphere_cb(rsl::make_unique<rsl::byte[]>(sizeof(glm::mat4))); // NOLINT(modernize-avoid-c-arrays)
      rex::memory::Blob right_sphere_cb(rsl::make_unique<rsl::byte[]>(sizeof(glm::mat4))); // NOLINT(modernize-avoid-c-arrays)

      left_cyl_cb.write(&left_cyl_world, rsl::memory_size(sizeof(left_cyl_world)));
      right_cyl_cb.write(&right_cyl_world, rsl::memory_size(sizeof(right_cyl_world)));
      left_sphere_cb.write(&left_sphere_world, rsl::memory_size(sizeof(left_sphere_world)));
      right_sphere_cb.write(&right_sphere_world, rsl::memory_size(sizeof(right_sphere_world)));

      rex::renderer::add_mesh(m_geometry.get(), cylinder, rex::memory::BlobView(left_cyl_cb));
      rex::renderer::add_mesh(m_geometry.get(), cylinder, rex::memory::BlobView(right_cyl_cb));
      rex::renderer::add_mesh(m_geometry.get(), sphere, rex::memory::BlobView(left_sphere_cb));
      rex::renderer::add_mesh(m_geometry.get(), sphere, rex::memory::BlobView(right_sphere_cb));
    }
  }

  rex::renderer::Submesh SampleScene::build_submesh(const rex::mesh_factory::MeshData16& meshData, rsl::vector<rex::renderer::VertexPosNormCol>& vertices, rsl::vector<u16>& indices) // NOLINT(readability-convert-member-functions-to-static)
  {
    rex::renderer::Submesh submesh;
    submesh.index_count          = meshData.indices().size();
    submesh.start_index_location = indices.size();
    submesh.base_vertex_location = vertices.size();

    vertices.insert(vertices.end(), meshData.vertices().cbegin(), meshData.vertices().cend());
    indices.insert(indices.end(), meshData.indices().cbegin(), meshData.indices().cend());

    return submesh;
  }

} // namespace regina

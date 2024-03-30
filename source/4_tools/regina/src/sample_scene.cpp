#include "regina/sample_scene.h"

#include "rex_engine/app/windowinfo.h"
#include "rex_engine/app/core_application.h"
#include "rex_windows/app/gui_application.h"
#include "rex_engine/primitives/mesh_factory.h"
#include "rex_engine/primitives/box.h"
#include "rex_engine/primitives/grid.h"
#include "rex_engine/primitives/sphere.h"
#include "rex_engine/primitives/cylinder.h"
#include "rex_renderer_core/resources/vertex.h"
#include "rex_renderer_core/system/renderer.h"
#include "rex_renderer_core/rendering/default_depth_info.h"
#include "rex_renderer_core/rendering/default_targets_info.h"
#include "rex_renderer_core/system/rhi.h"

#include "rex_std/bonus/math/color.h"

#include <glm/gtc/matrix_transform.hpp>

namespace regina
{
  SampleScene::SampleScene()
  {
    build_geometry();

    build_shader("regina\\Shaders\\color.hlsl", "regina\\Shaders\\color.hlsl");
    build_input_layout();

    build_raster_state();
    build_pso();

    f32 width = rex::globals::window_info().width;
    f32 height = rex::globals::window_info().height;
    build_constant_buffers(width, height);

    use_pso();
  }

  void SampleScene::update_object_constant_buffers()
  {
    // Nothing to implement
  }

  void SampleScene::build_geometry()
  {
    // Create the vertices and indices for all objects we want to render
    rsl::vector<rex::renderer::VertexPosCol> vertices;
    rsl::vector<u16> indices;

    rex::renderer::Submesh box       = build_submesh(rex::mesh_factory::create_box(1.5f, 1.5f, 1.5f, 0), vertices, indices);
    rex::renderer::Submesh grid      = build_submesh(rex::mesh_factory::create_grid(20.0f, 30.0f, 60, 40), vertices, indices);
    rex::renderer::Submesh sphere    = build_submesh(rex::mesh_factory::create_sphere(0.5f, 20, 20), vertices, indices);
    rex::renderer::Submesh cylinder  = build_submesh(rex::mesh_factory::create_cylinder(0.5f, 0.3f, 3.0f, 20, 20), vertices, indices);

    // Create the descs for the vertex buffer and index buffer
    m_vb = rex::memory::Blob(rsl::make_unique<rex::renderer::VertexPosCol[]>(vertices.size()));
    m_vb.write(vertices.data(), rsl::memory_size(m_vb.size()));
    rex::rhi::VertexBufferDesc vb_desc{ rex::memory::BlobView(m_vb), sizeof(rex::renderer::VertexPosCol) };

    m_ib = rex::memory::Blob(rsl::make_unique<u16[]>(indices.size()));
    m_ib.write(indices.data(), m_ib.size());
    rex::rhi::IndexBufferDesc ib_desc{ rex::memory::BlobView(m_ib), rex::renderer::IndexBufferFormat::Uint16, indices.size() };

    // Create the mesh geometry
    m_geometry = rsl::make_unique<rex::renderer::Mesh>("scene_geometry"_med, vb_desc, ib_desc);

    m_geometry->add_submesh("box"_small, box);
    m_geometry->add_submesh("grid"_small, grid);
    m_geometry->add_submesh("sphere"_small, sphere);
    m_geometry->add_submesh("cylinder"_small, cylinder);

    // Notify the renderer to draw the meshes
    rex::renderer::RenderItem* render_item = nullptr;

    // Box
    glm::mat4 world = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    world = glm::transpose(world); // Always transpose as we're working with Direct X

    rex::memory::Blob box_cb(rsl::make_unique<rsl::byte[]>(sizeof(world)));
    box_cb.write(&world, rsl::memory_size(sizeof(world)));
    render_item = rex::renderer::add_mesh(m_geometry.get(), box, rex::memory::BlobView(box_cb));
    rex::rhi::update_buffer(render_item->cb(), &world, sizeof(world));

    // Grid
    glm::mat4 grid_trans = glm::mat4(1.0f);
    grid_trans = glm::transpose(grid_trans); // Always transpose as we're working with Direct X
    rex::memory::Blob grid_cb(rsl::make_unique<rsl::byte[]>(sizeof(grid_trans)));
    grid_cb.write(&grid_trans, rsl::memory_size(sizeof(grid_trans)));
    render_item = rex::renderer::add_mesh(m_geometry.get(), grid, rex::memory::BlobView(grid_cb));
    rex::rhi::update_buffer(render_item->cb(), &grid_trans, sizeof(grid_trans));

     //Spheres and Cylinders
    for (s32 i = 0; i < 5; ++i)
    {
      glm::mat4 left_cyl_world = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 1.5f, -10.0f + i * 5.0f));
      left_cyl_world = glm::transpose(left_cyl_world); // Always transpose as we're working with Direct X
      glm::mat4 right_cyl_world = glm::translate(glm::mat4(1.0f), glm::vec3(+5.0f, 1.5f, -10.0f + i * 5.0f));
      right_cyl_world = glm::transpose(right_cyl_world); // Always transpose as we're working with Direct X
      glm::mat4 left_sphere_world = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 3.5f, -10.0f + i * 5.0f));
      left_sphere_world = glm::transpose(left_sphere_world); // Always transpose as we're working with Direct X
      glm::mat4 right_sphere_world = glm::translate(glm::mat4(1.0f), glm::vec3(+5.0f, 3.5f, -10.0f + i * 5.0f));
      right_sphere_world = glm::transpose(right_sphere_world); // Always transpose as we're working with Direct X

      rex::memory::Blob left_cyl_cb(rsl::make_unique<rsl::byte[]>(sizeof(glm::mat4)));
      rex::memory::Blob right_cyl_cb(rsl::make_unique<rsl::byte[]>(sizeof(glm::mat4)));
      rex::memory::Blob left_sphere_cb(rsl::make_unique<rsl::byte[]>(sizeof(glm::mat4)));
      rex::memory::Blob right_sphere_cb(rsl::make_unique<rsl::byte[]>(sizeof(glm::mat4)));

      left_cyl_cb.write(&left_cyl_world, rsl::memory_size(sizeof(left_cyl_world)));
      right_cyl_cb.write(&right_cyl_world, rsl::memory_size(sizeof(right_cyl_world)));
      left_sphere_cb.write(&left_sphere_world, rsl::memory_size(sizeof(left_sphere_world)));
      right_sphere_cb.write(&right_sphere_world, rsl::memory_size(sizeof(right_sphere_world)));

      render_item = rex::renderer::add_mesh(m_geometry.get(), cylinder, rex::memory::BlobView(left_cyl_cb));
      rex::rhi::update_buffer(render_item->cb(), &left_cyl_world, sizeof(left_cyl_world));
      render_item = rex::renderer::add_mesh(m_geometry.get(), cylinder, rex::memory::BlobView(right_cyl_cb));
      rex::rhi::update_buffer(render_item->cb(), &right_cyl_world, sizeof(right_cyl_world));
      render_item = rex::renderer::add_mesh(m_geometry.get(), sphere, rex::memory::BlobView(left_sphere_cb));
      rex::rhi::update_buffer(render_item->cb(), &left_sphere_world, sizeof(left_sphere_world));
      render_item = rex::renderer::add_mesh(m_geometry.get(), sphere, rex::memory::BlobView(right_sphere_cb));
      rex::rhi::update_buffer(render_item->cb(), &right_sphere_world, sizeof(right_sphere_world));
    }
  }

  rex::renderer::Submesh SampleScene::build_submesh(const rex::mesh_factory::MeshData16& meshData, rsl::vector<rex::renderer::VertexPosCol>& vertices, rsl::vector<u16>& indices)
  {
    rex::renderer::Submesh submesh;
    submesh.index_count = meshData.indices().size();
    submesh.start_index_location = indices.size();
    submesh.base_vertex_location = vertices.size();

    rsl::vector<rex::renderer::VertexPosCol> meshdata_vertices{};
    for (const rex::mesh_factory::Vertex& vtx : meshData.vertices())
    {
      meshdata_vertices.push_back(rex::renderer::VertexPosCol(vtx.position, glm::vec4(vtx.normal, 1.0f)));
    }
    rsl::vector<u16> meshdata_indices{};
    for (u16 idx : meshData.indices())
    {
      meshdata_indices.push_back(idx + submesh.base_vertex_location);
    }

    vertices.insert(vertices.end(), meshdata_vertices.cbegin(), meshdata_vertices.cend());
    indices.insert(indices.end(), meshdata_indices.cbegin(), meshdata_indices.cend());

    return submesh;
  }

}

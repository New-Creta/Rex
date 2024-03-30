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
    rex::renderer::VertexBufferDesc vb_desc{ rex::memory::BlobView(m_vb), sizeof(rex::renderer::VertexPosCol) };

    m_ib = rex::memory::Blob(rsl::make_unique<u16[]>(indices.size()));
    m_ib.write(indices.data(), m_ib.size());
    rex::renderer::IndexBufferDesc ib_desc{ rex::memory::BlobView(m_ib), rex::renderer::IndexBufferFormat::Uint16, indices.size() };

    // Create the mesh geometry
    m_geometry = rsl::make_unique<rex::renderer::Mesh>("scene_geometry"_med, vb_desc, ib_desc);

    m_geometry->add_submesh("box"_small, box);
    m_geometry->add_submesh("grid"_small, grid);
    m_geometry->add_submesh("sphere"_small, sphere);
    m_geometry->add_submesh("cylinder"_small, cylinder);

    // Notify the renderer to draw the meshes


    // Create the world positions of all of these so we can create the constant buffers for them

    // Box
    m_cube_world = glm::mat4(1.0f);
    m_cube_world = glm::scale(m_cube_world, glm::vec3(2.0f, 2.0f, 2.0f));

    rex::memory::Blob box_cb(rsl::make_unique<rsl::byte[]>(sizeof(m_cube_world)));
    box_cb.write(&m_cube_world, rsl::memory_size(sizeof(m_cube_world)));
    m_cube_render_item = rex::renderer::add_mesh(m_geometry.get(), box, rex::memory::BlobView(box_cb));
    rex::rhi::update_buffer(m_cube_render_item->cb(), &m_cube_world, sizeof(m_cube_world));

    // Grid
    const glm::mat4 grid_trans = glm::mat4(1.0f);
    rex::memory::Blob grid_cb(rsl::make_unique<rsl::byte[]>(sizeof(grid_trans)));
    grid_cb.write(&grid_trans, rsl::memory_size(sizeof(grid_trans)));
    rex::renderer::add_mesh(m_geometry.get(), grid, rex::memory::BlobView(grid_cb));

     //Spheres and Cylinders
    for (s32 i = 0; i < 5; ++i)
    {
      glm::mat4 left_cyl_world = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 1.5f, -10.0f + i * 5.0f));
      left_cyl_world = glm::transpose(left_cyl_world);
      glm::mat4 right_cyl_world = glm::translate(glm::mat4(1.0f), glm::vec3(+5.0f, 1.5f, -10.0f + i * 5.0f));
      right_cyl_world = glm::transpose(right_cyl_world);
      glm::mat4 left_sphere_world = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 3.5f, -10.0f + i * 5.0f));
      left_sphere_world = glm::transpose(left_sphere_world);
      glm::mat4 right_sphere_world = glm::translate(glm::mat4(1.0f), glm::vec3(+5.0f, 3.5f, -10.0f + i * 5.0f));
      right_sphere_world = glm::transpose(right_sphere_world);

      rex::memory::Blob left_cyl_cb(rsl::make_unique<rsl::byte[]>(sizeof(glm::mat4)));
      rex::memory::Blob right_cyl_cb(rsl::make_unique<rsl::byte[]>(sizeof(glm::mat4)));
      rex::memory::Blob left_sphere_cb(rsl::make_unique<rsl::byte[]>(sizeof(glm::mat4)));
      rex::memory::Blob right_sphere_cb(rsl::make_unique<rsl::byte[]>(sizeof(glm::mat4)));

      left_cyl_cb.write(&left_cyl_world, rsl::memory_size(sizeof(left_cyl_world)));
      right_cyl_cb.write(&right_cyl_world, rsl::memory_size(sizeof(right_cyl_world)));
      left_sphere_cb.write(&left_sphere_world, rsl::memory_size(sizeof(left_sphere_world)));
      right_sphere_cb.write(&right_sphere_world, rsl::memory_size(sizeof(right_sphere_world)));

      rex::renderer::RenderItem* ri = nullptr;
      ri = rex::renderer::add_mesh(m_geometry.get(), cylinder, rex::memory::BlobView(left_cyl_cb));
      rex::rhi::update_buffer(ri->cb(), &left_cyl_world, sizeof(left_cyl_world));
      ri = rex::renderer::add_mesh(m_geometry.get(), cylinder, rex::memory::BlobView(right_cyl_cb));
      rex::rhi::update_buffer(ri->cb(), &right_cyl_world, sizeof(right_cyl_world));
      ri = rex::renderer::add_mesh(m_geometry.get(), sphere, rex::memory::BlobView(left_sphere_cb));
      rex::rhi::update_buffer(ri->cb(), &left_sphere_world, sizeof(left_sphere_world));
      ri = rex::renderer::add_mesh(m_geometry.get(), sphere, rex::memory::BlobView(right_sphere_cb));
      rex::rhi::update_buffer(ri->cb(), &right_sphere_world, sizeof(right_sphere_world));
    }











    //auto grid = rex::mesh_factory::create_grid(20.0f, 30.0f, 60, 40);
    //auto sphere = rex::mesh_factory::create_sphere(0.5f, 20, 20);
    //auto cylinder = rex::mesh_factory::create_cylinder(0.5f, 0.3f, 3.0f, 20, 20);

    ////
    //// We are concatenating all the geometry into one big vertex/index buffer.  So
    //// define the regions in the buffer each submesh covers.
    ////

    //// Cache the vertex offsets to each object in the concatenated vertex buffer.
    //const u32 box_vertex_offset = 0;
    //const u32 grid_vertex_offset = box.vertices().size();
    //const u32 sphere_verte_x_offset = grid_vertex_offset + grid.vertices().size();
    //const u32 cylinder_vertex_offset = sphere_verte_x_offset + sphere.vertices().size();

    //// Cache the starting index for each object in the concatenated index buffer.
    //const u32 box_index_offset = 0;
    //const u32 grid_index_offset = box.indices().size();
    //const u32 sphere_index_offset = grid_index_offset + grid.indices().size();
    //const u32 cylinder_index_offset = sphere_index_offset + sphere.indices().size();

    //// Define the SubmeshGeometry that cover different
    //// regions of the vertex/index buffers.

    //rex::renderer::Submesh box_submesh;
    //box_submesh.index_count = box.indices().size();
    //box_submesh.start_index_location = box_index_offset;
    //box_submesh.base_vertex_location = box_vertex_offset;

    //rex::renderer::Submesh grid_submesh;
    //grid_submesh.index_count = grid.indices().size();
    //grid_submesh.start_index_location = grid_index_offset;
    //grid_submesh.base_vertex_location = grid_vertex_offset;

    //rex::renderer::Submesh sphere_submesh;
    //sphere_submesh.index_count = sphere.indices().size();
    //sphere_submesh.start_index_location = sphere_index_offset;
    //sphere_submesh.base_vertex_location = sphere_verte_x_offset;

    //rex::renderer::Submesh cylinder_submesh;
    //cylinder_submesh.index_count = cylinder.indices().size();
    //cylinder_submesh.start_index_location = cylinder_index_offset;
    //cylinder_submesh.base_vertex_location = cylinder_vertex_offset;

    ////
    //// Extract the vertex elements we are interested in and pack the
    //// vertices of all the meshes into one vertex buffer.
    ////

    //auto total_vertex_count = box.vertices().size() + grid.vertices().size() + sphere.vertices().size() + cylinder.vertices().size();
    //auto total_index_count = box.indices().size() + grid.indices().size() + sphere.indices().size() + cylinder.indices().size();

    //rsl::unique_array<rex::renderer::VertexPosCol> vertices = rsl::make_unique<rex::renderer::VertexPosCol[]>(total_vertex_count);

    //u32 k = 0;
    //for (s32 i = 0; i < box.vertices().size(); ++i, ++k)
    //{
    //  const glm::vec3 position = box.vertices()[i].position;
    //  auto c = rsl::colors::OrangeRed;
    //  const glm::vec4 color = { c.red, c.green, c.blue, c.alpha };

    //  vertices[k] = rex::renderer::VertexPosCol(position, color);
    //}

    //for (s32 i = 0; i < grid.vertices().size(); ++i, ++k)
    //{
    //  const glm::vec3 position = grid.vertices()[i].position;
    //  auto c = rsl::colors::ForestGreen;
    //  const glm::vec4 color = { c.red, c.green, c.blue, c.alpha };

    //  vertices[k] = rex::renderer::VertexPosCol(position, color);
    //}

    //for (s32 i = 0; i < sphere.vertices().size(); ++i, ++k)
    //{
    //  const glm::vec3 position = sphere.vertices()[i].position;
    //  auto c = rsl::colors::Crimson;
    //  const glm::vec4 color = { c.red, c.green, c.blue, c.alpha };

    //  vertices[k] = rex::renderer::VertexPosCol(position, color);
    //}

    //for (s32 i = 0; i < cylinder.vertices().size(); ++i, ++k)
    //{
    //  const glm::vec3 position = cylinder.vertices()[i].position;
    //  auto c = rsl::colors::SteelBlue;
    //  const glm::vec4 color = { c.red, c.green, c.blue, c.alpha };

    //  vertices[k] = rex::renderer::VertexPosCol(position, color);
    //}

    //rsl::vector<u16> indices;
    //indices.insert(indices.end(), rsl::begin(box.indices()), rsl::end(box.indices()));
    //indices.insert(indices.end(), rsl::begin(grid.indices()), rsl::end(grid.indices()));
    //indices.insert(indices.end(), rsl::begin(sphere.indices()), rsl::end(sphere.indices()));
    //indices.insert(indices.end(), rsl::begin(cylinder.indices()), rsl::end(cylinder.indices()));
    //rsl::unique_array<rsl::byte> indices_data = rsl::make_unique<rsl::byte[]>(indices.size());
    //memcpy(indices_data.get(), indices.data(), indices.size() * sizeof(decltype(indices)::value_type));
    //rex::memory::Blob indices_blob(rsl::move(indices_data));

    //const u32 vb_byte_size = total_vertex_count * sizeof(rex::renderer::VertexPosCol);
    //const u32 ib_byte_size = total_index_count * sizeof(u16);

    //rex::renderer::VertexBufferDesc vb_desc(rsl::move(vertices));
    //rex::renderer::IndexBufferDesc ib_desc{rsl::move(indices_blob), rex::renderer::IndexBufferFormat::Uint16};
    //rex::renderer::ConstantBufferDesc cb_desc{ rsl::move(constant_buffer) };

    //auto geometry = rsl::make_unique<rex::renderer::Mesh>("scene_geometry"_med, rsl::move(vb_desc), rsl::move(ib_desc), rsl::move(cb_desc));

    //geometry->add_submesh("box"_small, box_submesh);
    //geometry->add_submesh("grid"_small, grid_submesh);
    //geometry->add_submesh("sphere"_small, sphere_submesh);
    //geometry->add_submesh("cylinder"_small, cylinder_submesh);

    //rex::renderer::add_mesh(geometry.get());
    //m_meshes.push_back(rsl::move(geometry));
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

  void SampleScene::update_object_constant_buffers()
  {
    //rex::rhi::update_buffer(m_cube_render_item->cb(), &m_cube_world, sizeof(m_cube_world));
  }

  //void SampleScene::build_render_items()
  //{
  //  const glm::mat4 box_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
  //  const glm::mat4 box_scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));

  //  auto box_r_item = rex::renderer::RenderItem();
  //  box_r_item.world = box_trans * box_scale;
  //  box_r_item.constant_buffer_index = 0;
  //  box_r_item.geometry = m_meshes["scene_geometry"_med].get();
  //  box_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
  //  box_r_item.index_count = box_r_item.geometry->submesh("box"_small)->index_count;
  //  box_r_item.start_index_location = box_r_item.geometry->submesh("box"_small)->start_index_location;
  //  box_r_item.base_vertex_location = box_r_item.geometry->submesh("box"_small)->base_vertex_location;
  //  box_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();
  //  add_render_item(rsl::move(box_r_item));

  //  auto grid_r_item = rex::renderer::RenderItem();
  //  grid_r_item.world = glm::mat4(1.0f);
  //  grid_r_item.constant_buffer_index = 1;
  //  grid_r_item.geometry = m_meshes["scene_geometry"_med].get();
  //  grid_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
  //  grid_r_item.index_count = grid_r_item.geometry->submesh("grid"_small)->index_count;
  //  grid_r_item.start_index_location = grid_r_item.geometry->submesh("grid"_small)->start_index_location;
  //  grid_r_item.base_vertex_location = grid_r_item.geometry->submesh("grid"_small)->base_vertex_location;
  //  grid_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();
  //  add_render_item(rsl::move(grid_r_item));

  //  u32 obj_cb_index = 2;
  //  for (int i = 0; i < 5; ++i)
  //  {
  //    auto left_cyl_r_item = rex::renderer::RenderItem();
  //    auto right_cyl_r_item = rex::renderer::RenderItem();
  //    auto left_sphere_r_item = rex::renderer::RenderItem();
  //    auto right_sphere_r_item = rex::renderer::RenderItem();

  //    const glm::mat4 left_cyl_world = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 1.5f, -10.0f + i * 5.0f));
  //    const glm::mat4 right_cyl_world = glm::translate(glm::mat4(1.0f), glm::vec3(+5.0f, 1.5f, -10.0f + i * 5.0f));
  //    const glm::mat4 left_sphere_world = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 3.5f, -10.0f + i * 5.0f));
  //    const glm::mat4 right_sphere_world = glm::translate(glm::mat4(1.0f), glm::vec3(+5.0f, 3.5f, -10.0f + i * 5.0f));

  //    left_cyl_r_item.world = right_cyl_world;
  //    left_cyl_r_item.constant_buffer_index = obj_cb_index++;
  //    left_cyl_r_item.geometry = m_meshes["scene_geometry"_med].get();
  //    left_cyl_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
  //    left_cyl_r_item.index_count = left_cyl_r_item.geometry->submesh("cylinder"_small)->index_count;
  //    left_cyl_r_item.start_index_location = left_cyl_r_item.geometry->submesh("cylinder"_small)->start_index_location;
  //    left_cyl_r_item.base_vertex_location = left_cyl_r_item.geometry->submesh("cylinder"_small)->base_vertex_location;
  //    left_cyl_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();

  //    right_cyl_r_item.world = left_cyl_world;
  //    right_cyl_r_item.constant_buffer_index = obj_cb_index++;
  //    right_cyl_r_item.geometry = m_meshes["scene_geometry"_med].get();
  //    right_cyl_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
  //    right_cyl_r_item.index_count = right_cyl_r_item.geometry->submesh("cylinder"_small)->index_count;
  //    right_cyl_r_item.start_index_location = right_cyl_r_item.geometry->submesh("cylinder"_small)->start_index_location;
  //    right_cyl_r_item.base_vertex_location = right_cyl_r_item.geometry->submesh("cylinder"_small)->base_vertex_location;
  //    right_cyl_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();

  //    left_sphere_r_item.world = left_sphere_world;
  //    left_sphere_r_item.constant_buffer_index = obj_cb_index++;
  //    left_sphere_r_item.geometry = m_meshes["scene_geometry"_med].get();
  //    left_sphere_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
  //    left_sphere_r_item.index_count = left_sphere_r_item.geometry->submesh("sphere"_small)->index_count;
  //    left_sphere_r_item.start_index_location = left_sphere_r_item.geometry->submesh("sphere"_small)->start_index_location;
  //    left_sphere_r_item.base_vertex_location = left_sphere_r_item.geometry->submesh("sphere"_small)->base_vertex_location;
  //    left_sphere_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();

  //    right_sphere_r_item.world = right_sphere_world;
  //    right_sphere_r_item.constant_buffer_index = obj_cb_index++;
  //    right_sphere_r_item.geometry = m_meshes["scene_geometry"_med].get();
  //    right_sphere_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
  //    right_sphere_r_item.index_count = right_sphere_r_item.geometry->submesh("sphere"_small)->index_count;
  //    right_sphere_r_item.start_index_location = right_sphere_r_item.geometry->submesh("sphere"_small)->start_index_location;
  //    right_sphere_r_item.base_vertex_location = right_sphere_r_item.geometry->submesh("sphere"_small)->base_vertex_location;
  //    right_sphere_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();

  //    add_render_item(rsl::move(left_cyl_r_item));
  //    add_render_item(rsl::move(right_cyl_r_item));
  //    add_render_item(rsl::move(left_sphere_r_item));
  //    add_render_item(rsl::move(right_sphere_r_item));
  //  }

  //}
}

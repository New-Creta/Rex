//#include "regina/sample_scene.h"
//
//#include "rex_engine/app/windowinfo.h"
//#include "rex_engine/app/core_application.h"
//#include "rex_windows/app/gui_application.h"
//#include "rex_engine/primitives/mesh_factory.h"
//#include "rex_engine/primitives/box.h"
//#include "rex_engine/primitives/grid.h"
//#include "rex_engine/primitives/sphere.h"
//#include "rex_engine/primitives/cylinder.h"
//#include "rex_renderer_core/resources/vertex.h"
//#include "rex_renderer_core/commands/create_buffer_cmd.h"
//#include "rex_renderer_core/commands/compile_shader_cmd.h"
//#include "rex_renderer_core/commands/link_shader_cmd.h"
//#include "rex_renderer_core/commands/create_input_layout_cmd.h"
//#include "rex_renderer_core/commands/create_raster_state_cmd.h"
//#include "rex_renderer_core/commands/create_pipeline_state_cmd.h"
//#include "rex_renderer_core/commands/update_committed_resource_cmd.h"
//#include "rex_renderer_core/system/renderer.h"
//#include "rex_renderer_core/rendering/default_depth_info.h"
//#include "rex_renderer_core/rendering/default_targets_info.h"
//
//#include "rex_std/bonus/math/color.h"
//
//#include <glm/gtc/matrix_transform.hpp>
//
//namespace regina
//{
//  SampleScene::SampleScene()
//  {
//    build_geometry();
//    build_render_items();
//
//    build_shader("regina\\Shaders\\color.hlsl", "regina\\Shaders\\color.hlsl");
//    build_input_layout();
//
//    build_raster_state();
//    build_pso();
//    build_frame_resources();
//    build_constant_buffers();
//
//    use_pso();
//  }
//
//  void SampleScene::build_geometry()
//  {
//    auto box = rex::mesh_factory::create_box(1.5f, 0.5f, 1.5f, 0);
//    auto grid = rex::mesh_factory::create_grid(20.0f, 30.0f, 60, 40);
//    auto sphere = rex::mesh_factory::create_sphere(0.5f, 20, 20);
//    auto cylinder = rex::mesh_factory::create_cylinder(0.5f, 0.3f, 3.0f, 20, 20);
//
//    //
//    // We are concatenating all the geometry into one big vertex/index buffer.  So
//    // define the regions in the buffer each submesh covers.
//    //
//
//    // Cache the vertex offsets to each object in the concatenated vertex buffer.
//    const u32 box_vertex_offset = 0;
//    const u32 grid_vertex_offset = box.vertices().size();
//    const u32 sphere_verte_x_offset = grid_vertex_offset + grid.vertices().size();
//    const u32 cylinder_vertex_offset = sphere_verte_x_offset + sphere.vertices().size();
//
//    // Cache the starting index for each object in the concatenated index buffer.
//    const u32 box_index_offset = 0;
//    const u32 grid_index_offset = box.indices().size();
//    const u32 sphere_index_offset = grid_index_offset + grid.indices().size();
//    const u32 cylinder_index_offset = sphere_index_offset + sphere.indices().size();
//
//    // Define the SubmeshGeometry that cover different
//    // regions of the vertex/index buffers.
//
//    rex::renderer::Submesh box_submesh;
//    box_submesh.index_count = box.indices().size();
//    box_submesh.start_index_location = box_index_offset;
//    box_submesh.base_vertex_location = box_vertex_offset;
//
//    rex::renderer::Submesh grid_submesh;
//    grid_submesh.index_count = grid.indices().size();
//    grid_submesh.start_index_location = grid_index_offset;
//    grid_submesh.base_vertex_location = grid_vertex_offset;
//
//    rex::renderer::Submesh sphere_submesh;
//    sphere_submesh.index_count = sphere.indices().size();
//    sphere_submesh.start_index_location = sphere_index_offset;
//    sphere_submesh.base_vertex_location = sphere_verte_x_offset;
//
//    rex::renderer::Submesh cylinder_submesh;
//    cylinder_submesh.index_count = cylinder.indices().size();
//    cylinder_submesh.start_index_location = cylinder_index_offset;
//    cylinder_submesh.base_vertex_location = cylinder_vertex_offset;
//
//    //
//    // Extract the vertex elements we are interested in and pack the
//    // vertices of all the meshes into one vertex buffer.
//    //
//
//    auto total_vertex_count = box.vertices().size() + grid.vertices().size() + sphere.vertices().size() + cylinder.vertices().size();
//    auto total_index_count = box.indices().size() + grid.indices().size() + sphere.indices().size() + cylinder.indices().size();
//
//    rsl::vector<rex::renderer::VertexPosCol> vertices((rsl::Size(total_vertex_count)));
//
//    u32 k = 0;
//    for (s32 i = 0; i < box.vertices().size(); ++i, ++k)
//    {
//      const glm::vec3 position = box.vertices()[i].position;
//      auto c = rsl::colors::OrangeRed;
//      const glm::vec4 color = { c.red, c.green, c.blue, c.alpha };
//
//      vertices[k] = rex::renderer::VertexPosCol(position, color);
//    }
//
//    for (s32 i = 0; i < grid.vertices().size(); ++i, ++k)
//    {
//      const glm::vec3 position = grid.vertices()[i].position;
//      auto c = rsl::colors::ForestGreen;
//      const glm::vec4 color = { c.red, c.green, c.blue, c.alpha };
//
//      vertices[k] = rex::renderer::VertexPosCol(position, color);
//    }
//
//    for (s32 i = 0; i < sphere.vertices().size(); ++i, ++k)
//    {
//      const glm::vec3 position = sphere.vertices()[i].position;
//      auto c = rsl::colors::Crimson;
//      const glm::vec4 color = { c.red, c.green, c.blue, c.alpha };
//
//      vertices[k] = rex::renderer::VertexPosCol(position, color);
//    }
//
//    for (s32 i = 0; i < cylinder.vertices().size(); ++i, ++k)
//    {
//      const glm::vec3 position = cylinder.vertices()[i].position;
//      auto c = rsl::colors::SteelBlue;
//      const glm::vec4 color = { c.red, c.green, c.blue, c.alpha };
//
//      vertices[k] = rex::renderer::VertexPosCol(position, color);
//    }
//
//    rsl::vector<u16> indices;
//    indices.insert(indices.end(), rsl::begin(box.indices()), rsl::end(box.indices()));
//    indices.insert(indices.end(), rsl::begin(grid.indices()), rsl::end(grid.indices()));
//    indices.insert(indices.end(), rsl::begin(sphere.indices()), rsl::end(sphere.indices()));
//    indices.insert(indices.end(), rsl::begin(cylinder.indices()), rsl::end(cylinder.indices()));
//
//    const u32 vb_byte_size = total_vertex_count * sizeof(rex::renderer::VertexPosCol);
//    const u32 ib_byte_size = total_index_count * sizeof(u16);
//
//     rex::renderer::commands::CreateBufferCommandDesc v_create_buffer_command_desc = rex::renderer::commands::create_buffer_parameters<rex::renderer::VertexPosCol>(vertices.data(), vertices.size());
//    rex::renderer::Mesh::VertexBufferDesc vbd(rex::renderer::create_vertex_buffer(rsl::move(v_create_buffer_command_desc)), sizeof(rex::renderer::VertexPosCol), vb_byte_size);
//
//    rex::renderer::commands::CreateBufferCommandDesc i_create_buffer_command_desc = rex::renderer::commands::create_buffer_parameters<u16>(indices.data(), indices.size());
//    rex::renderer::Mesh::IndexBufferDesc ibd(rex::renderer::create_index_buffer(rsl::move(i_create_buffer_command_desc)), rex::renderer::IndexBufferFormat::R16Uint, ib_byte_size);
//
//    auto geometry = rsl::make_unique<rex::renderer::Mesh>("scene_geometry"_med, vbd, ibd);
//
//    geometry->add_submesh("box"_small, box_submesh);
//    geometry->add_submesh("grid"_small, grid_submesh);
//    geometry->add_submesh("sphere"_small, sphere_submesh);
//    geometry->add_submesh("cylinder"_small, cylinder_submesh);
//
//    m_meshes[geometry->name()] = rsl::move(geometry);
//
//  }
//
//  void SampleScene::build_render_items()
//  {
//    const glm::mat4 box_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
//    const glm::mat4 box_scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
//
//    auto box_r_item = rex::renderer::RenderItem();
//    box_r_item.world = box_trans * box_scale;
//    box_r_item.constant_buffer_index = 0;
//    box_r_item.geometry = m_meshes["scene_geometry"_med].get();
//    box_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
//    box_r_item.index_count = box_r_item.geometry->submesh("box"_small)->index_count;
//    box_r_item.start_index_location = box_r_item.geometry->submesh("box"_small)->start_index_location;
//    box_r_item.base_vertex_location = box_r_item.geometry->submesh("box"_small)->base_vertex_location;
//    box_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();
//    add_render_item(rsl::move(box_r_item));
//
//    auto grid_r_item = rex::renderer::RenderItem();
//    grid_r_item.world = glm::mat4(1.0f);
//    grid_r_item.constant_buffer_index = 1;
//    grid_r_item.geometry = m_meshes["scene_geometry"_med].get();
//    grid_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
//    grid_r_item.index_count = grid_r_item.geometry->submesh("grid"_small)->index_count;
//    grid_r_item.start_index_location = grid_r_item.geometry->submesh("grid"_small)->start_index_location;
//    grid_r_item.base_vertex_location = grid_r_item.geometry->submesh("grid"_small)->base_vertex_location;
//    grid_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();
//    add_render_item(rsl::move(grid_r_item));
//
//    u32 obj_cb_index = 2;
//    for (int i = 0; i < 5; ++i)
//    {
//      auto left_cyl_r_item = rex::renderer::RenderItem();
//      auto right_cyl_r_item = rex::renderer::RenderItem();
//      auto left_sphere_r_item = rex::renderer::RenderItem();
//      auto right_sphere_r_item = rex::renderer::RenderItem();
//
//      const glm::mat4 left_cyl_world = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 1.5f, -10.0f + i * 5.0f));
//      const glm::mat4 right_cyl_world = glm::translate(glm::mat4(1.0f), glm::vec3(+5.0f, 1.5f, -10.0f + i * 5.0f));
//      const glm::mat4 left_sphere_world = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 3.5f, -10.0f + i * 5.0f));
//      const glm::mat4 right_sphere_world = glm::translate(glm::mat4(1.0f), glm::vec3(+5.0f, 3.5f, -10.0f + i * 5.0f));
//
//      left_cyl_r_item.world = right_cyl_world;
//      left_cyl_r_item.constant_buffer_index = obj_cb_index++;
//      left_cyl_r_item.geometry = m_meshes["scene_geometry"_med].get();
//      left_cyl_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
//      left_cyl_r_item.index_count = left_cyl_r_item.geometry->submesh("cylinder"_small)->index_count;
//      left_cyl_r_item.start_index_location = left_cyl_r_item.geometry->submesh("cylinder"_small)->start_index_location;
//      left_cyl_r_item.base_vertex_location = left_cyl_r_item.geometry->submesh("cylinder"_small)->base_vertex_location;
//      left_cyl_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();
//
//      right_cyl_r_item.world = left_cyl_world;
//      right_cyl_r_item.constant_buffer_index = obj_cb_index++;
//      right_cyl_r_item.geometry = m_meshes["scene_geometry"_med].get();
//      right_cyl_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
//      right_cyl_r_item.index_count = right_cyl_r_item.geometry->submesh("cylinder"_small)->index_count;
//      right_cyl_r_item.start_index_location = right_cyl_r_item.geometry->submesh("cylinder"_small)->start_index_location;
//      right_cyl_r_item.base_vertex_location = right_cyl_r_item.geometry->submesh("cylinder"_small)->base_vertex_location;
//      right_cyl_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();
//
//      left_sphere_r_item.world = left_sphere_world;
//      left_sphere_r_item.constant_buffer_index = obj_cb_index++;
//      left_sphere_r_item.geometry = m_meshes["scene_geometry"_med].get();
//      left_sphere_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
//      left_sphere_r_item.index_count = left_sphere_r_item.geometry->submesh("sphere"_small)->index_count;
//      left_sphere_r_item.start_index_location = left_sphere_r_item.geometry->submesh("sphere"_small)->start_index_location;
//      left_sphere_r_item.base_vertex_location = left_sphere_r_item.geometry->submesh("sphere"_small)->base_vertex_location;
//      left_sphere_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();
//
//      right_sphere_r_item.world = right_sphere_world;
//      right_sphere_r_item.constant_buffer_index = obj_cb_index++;
//      right_sphere_r_item.geometry = m_meshes["scene_geometry"_med].get();
//      right_sphere_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
//      right_sphere_r_item.index_count = right_sphere_r_item.geometry->submesh("sphere"_small)->index_count;
//      right_sphere_r_item.start_index_location = right_sphere_r_item.geometry->submesh("sphere"_small)->start_index_location;
//      right_sphere_r_item.base_vertex_location = right_sphere_r_item.geometry->submesh("sphere"_small)->base_vertex_location;
//      right_sphere_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();
//
//      add_render_item(rsl::move(left_cyl_r_item));
//      add_render_item(rsl::move(right_cyl_r_item));
//      add_render_item(rsl::move(left_sphere_r_item));
//      add_render_item(rsl::move(right_sphere_r_item));
//    }
//
//  }
//}

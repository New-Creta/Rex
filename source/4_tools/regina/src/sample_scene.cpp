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
#include "rex_renderer_core/commands/create_buffer_cmd.h"
#include "rex_renderer_core/commands/compile_shader_cmd.h"
#include "rex_renderer_core/commands/link_shader_cmd.h"
#include "rex_renderer_core/commands/create_input_layout_cmd.h"
#include "rex_renderer_core/commands/create_raster_state_cmd.h"
#include "rex_renderer_core/commands/create_pipeline_state_cmd.h"
#include "rex_renderer_core/commands/update_committed_resource_cmd.h"
#include "rex_renderer_core/rendering/renderer.h"
#include "rex_renderer_core/rendering/default_depth_info.h"
#include "rex_renderer_core/rendering/default_targets_info.h"

#include "rex_std/bonus/math/color.h"

#include <glm/gtc/matrix_transform.hpp>

namespace regina
{
  struct ObjectConstants
  {
    glm::mat4 world = glm::mat4(1.0f);
  };

  SampleScene::SampleScene()
  {
    build_geometry();
    build_render_items();

    build_shader();
    build_raster_state();
    build_pso();
    build_frame_resources();
    build_constant_buffers();

    rex::renderer::set_pipeline_state_object(m_pso);
    rex::renderer::set_raster_state(m_raster_state);
  }

  void SampleScene::render(rex::renderer::SceneRenderer* renderer)
  {
    // This is a fancy way of saying "Get the resource slot of the gpu resource that's already on the gpu, which has memory available for the objects constant buffers"
    const rex::renderer::ResourceSlot curr_object_cr = get_object_committed_resource_of_frame(rex::renderer::active_frame());

    // Update view needs to be done through the camera
    // We don't have a camera class yet, it's static anyway
    // so we can just do it in the scene at the moment
    update_view();
    update_object_constant_buffers(curr_object_cr);
    update_pass_constant_buffers();

    // We have a global shader for the scene at the moment
    // In the future, this will be set on an individual model
    rex::renderer::set_shader(m_shader_program);

    // TODO: The following needs to be cleaned up, setting the constant buffer needs to be done better
    rex::renderer::ResourceSlot const curr_pass_cb = get_active_pass_constant_buffer_for_frame(rex::renderer::active_frame()->slot_id());
    rex::renderer::set_constant_buffer_view(curr_pass_cb, 1);

    rsl::vector<rex::renderer::ResourceSlot> contant_buffers = get_active_constant_buffer_for_frame(rex::renderer::active_frame()->slot_id());
    render_items(renderer, contant_buffers);

    // This also needs to be done by the camera, but like before
    // We don't have a camera class at the moment and it's static anyway
    // So we let the scene handle it at the moment.
    // The window resized, so update the aspect ratio and recompute the projection matrix.
    const auto fov = 0.25f * glm::pi<f32>();
    const auto width = static_cast<f32>(rex::globals::window_info().width);
    const auto height = static_cast<f32>(rex::globals::window_info().height);
    const auto near_plane = rex::globals::default_depth_info().near_plane;
    const auto far_plane = rex::globals::default_depth_info().far_plane;
    m_proj = glm::perspectiveFov(fov, width, height, near_plane, far_plane);
    m_proj = glm::transpose(m_proj); // DirectX backend ( so we have to transpose, expects row major matrices )
  }

  void SampleScene::build_geometry()
  {
    auto box = rex::mesh_factory::create_box(1.5f, 0.5f, 1.5f, 0);
    auto grid = rex::mesh_factory::create_grid(20.0f, 30.0f, 60, 40);
    auto sphere = rex::mesh_factory::create_sphere(0.5f, 20, 20);
    auto cylinder = rex::mesh_factory::create_cylinder(0.5f, 0.3f, 3.0f, 20, 20);

    //
    // We are concatenating all the geometry into one big vertex/index buffer.  So
    // define the regions in the buffer each submesh covers.
    //

    // Cache the vertex offsets to each object in the concatenated vertex buffer.
    const u32 box_vertex_offset = 0;
    const u32 grid_vertex_offset = box.vertices().size();
    const u32 sphere_verte_x_offset = grid_vertex_offset + grid.vertices().size();
    const u32 cylinder_vertex_offset = sphere_verte_x_offset + sphere.vertices().size();

    // Cache the starting index for each object in the concatenated index buffer.
    const u32 box_index_offset = 0;
    const u32 grid_index_offset = box.indices().size();
    const u32 sphere_index_offset = grid_index_offset + grid.indices().size();
    const u32 cylinder_index_offset = sphere_index_offset + sphere.indices().size();

    // Define the SubmeshGeometry that cover different
    // regions of the vertex/index buffers.

    rex::renderer::Submesh box_submesh;
    box_submesh.index_count = box.indices().size();
    box_submesh.start_index_location = box_index_offset;
    box_submesh.base_vertex_location = box_vertex_offset;

    rex::renderer::Submesh grid_submesh;
    grid_submesh.index_count = grid.indices().size();
    grid_submesh.start_index_location = grid_index_offset;
    grid_submesh.base_vertex_location = grid_vertex_offset;

    rex::renderer::Submesh sphere_submesh;
    sphere_submesh.index_count = sphere.indices().size();
    sphere_submesh.start_index_location = sphere_index_offset;
    sphere_submesh.base_vertex_location = sphere_verte_x_offset;

    rex::renderer::Submesh cylinder_submesh;
    cylinder_submesh.index_count = cylinder.indices().size();
    cylinder_submesh.start_index_location = cylinder_index_offset;
    cylinder_submesh.base_vertex_location = cylinder_vertex_offset;

    //
    // Extract the vertex elements we are interested in and pack the
    // vertices of all the meshes into one vertex buffer.
    //

    auto total_vertex_count = box.vertices().size() + grid.vertices().size() + sphere.vertices().size() + cylinder.vertices().size();
    auto total_index_count = box.indices().size() + grid.indices().size() + sphere.indices().size() + cylinder.indices().size();

    rsl::vector<rex::renderer::VertexPosCol> vertices((rsl::Size(total_vertex_count)));

    u32 k = 0;
    for (s32 i = 0; i < box.vertices().size(); ++i, ++k)
    {
      const glm::vec3 position = box.vertices()[i].position;
      auto c = rsl::colors::OrangeRed;
      const glm::vec4 color = { c.red, c.green, c.blue, c.alpha };

      vertices[k] = rex::renderer::VertexPosCol(position, color);
    }

    for (s32 i = 0; i < grid.vertices().size(); ++i, ++k)
    {
      const glm::vec3 position = grid.vertices()[i].position;
      auto c = rsl::colors::ForestGreen;
      const glm::vec4 color = { c.red, c.green, c.blue, c.alpha };

      vertices[k] = rex::renderer::VertexPosCol(position, color);
    }

    for (s32 i = 0; i < sphere.vertices().size(); ++i, ++k)
    {
      const glm::vec3 position = sphere.vertices()[i].position;
      auto c = rsl::colors::Crimson;
      const glm::vec4 color = { c.red, c.green, c.blue, c.alpha };

      vertices[k] = rex::renderer::VertexPosCol(position, color);
    }

    for (s32 i = 0; i < cylinder.vertices().size(); ++i, ++k)
    {
      const glm::vec3 position = cylinder.vertices()[i].position;
      auto c = rsl::colors::SteelBlue;
      const glm::vec4 color = { c.red, c.green, c.blue, c.alpha };

      vertices[k] = rex::renderer::VertexPosCol(position, color);
    }

    rsl::vector<u16> indices;
    indices.insert(indices.end(), rsl::begin(box.indices()), rsl::end(box.indices()));
    indices.insert(indices.end(), rsl::begin(grid.indices()), rsl::end(grid.indices()));
    indices.insert(indices.end(), rsl::begin(sphere.indices()), rsl::end(sphere.indices()));
    indices.insert(indices.end(), rsl::begin(cylinder.indices()), rsl::end(cylinder.indices()));

    const u32 vb_byte_size = total_vertex_count * sizeof(rex::renderer::VertexPosCol);
    const u32 ib_byte_size = total_index_count * sizeof(u16);

    rex::renderer::commands::CreateBufferCommandDesc v_create_buffer_command_desc = rex::renderer::commands::create_buffer_parameters<rex::renderer::VertexPosCol>(vertices.data(), vertices.size());
    rex::renderer::Mesh::VertexBufferDesc vbd(rex::renderer::create_vertex_buffer(rsl::move(v_create_buffer_command_desc)), sizeof(rex::renderer::VertexPosCol), vb_byte_size);

    rex::renderer::commands::CreateBufferCommandDesc i_create_buffer_command_desc = rex::renderer::commands::create_buffer_parameters<u16>(indices.data(), indices.size());
    rex::renderer::Mesh::IndexBufferDesc ibd(rex::renderer::create_index_buffer(rsl::move(i_create_buffer_command_desc)), rex::renderer::IndexBufferFormat::R16Uint, ib_byte_size);

    auto geometry = rsl::make_unique<rex::renderer::Mesh>("scene_geometry"_med, vbd, ibd);

    geometry->add_submesh("box"_small, box_submesh);
    geometry->add_submesh("grid"_small, grid_submesh);
    geometry->add_submesh("sphere"_small, sphere_submesh);
    geometry->add_submesh("cylinder"_small, cylinder_submesh);

    m_meshes[geometry->name()] = rsl::move(geometry);

  }

  void SampleScene::build_render_items()
  {
    const glm::mat4 box_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
    const glm::mat4 box_scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));

    auto box_r_item = rex::renderer::RenderItem();
    box_r_item.world = box_trans * box_scale;
    box_r_item.constant_buffer_index = 0;
    box_r_item.geometry = m_meshes["scene_geometry"_med].get();
    box_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
    box_r_item.index_count = box_r_item.geometry->submesh("box"_small)->index_count;
    box_r_item.start_index_location = box_r_item.geometry->submesh("box"_small)->start_index_location;
    box_r_item.base_vertex_location = box_r_item.geometry->submesh("box"_small)->base_vertex_location;
    box_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();
    add_render_item(rsl::move(box_r_item));

    auto grid_r_item = rex::renderer::RenderItem();
    grid_r_item.world = glm::mat4(1.0f);
    grid_r_item.constant_buffer_index = 1;
    grid_r_item.geometry = m_meshes["scene_geometry"_med].get();
    grid_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
    grid_r_item.index_count = grid_r_item.geometry->submesh("grid"_small)->index_count;
    grid_r_item.start_index_location = grid_r_item.geometry->submesh("grid"_small)->start_index_location;
    grid_r_item.base_vertex_location = grid_r_item.geometry->submesh("grid"_small)->base_vertex_location;
    grid_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();
    add_render_item(rsl::move(grid_r_item));

    u32 obj_cb_index = 2;
    for (int i = 0; i < 5; ++i)
    {
      auto left_cyl_r_item = rex::renderer::RenderItem();
      auto right_cyl_r_item = rex::renderer::RenderItem();
      auto left_sphere_r_item = rex::renderer::RenderItem();
      auto right_sphere_r_item = rex::renderer::RenderItem();

      const glm::mat4 left_cyl_world = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 1.5f, -10.0f + i * 5.0f));
      const glm::mat4 right_cyl_world = glm::translate(glm::mat4(1.0f), glm::vec3(+5.0f, 1.5f, -10.0f + i * 5.0f));
      const glm::mat4 left_sphere_world = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 3.5f, -10.0f + i * 5.0f));
      const glm::mat4 right_sphere_world = glm::translate(glm::mat4(1.0f), glm::vec3(+5.0f, 3.5f, -10.0f + i * 5.0f));

      left_cyl_r_item.world = right_cyl_world;
      left_cyl_r_item.constant_buffer_index = obj_cb_index++;
      left_cyl_r_item.geometry = m_meshes["scene_geometry"_med].get();
      left_cyl_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
      left_cyl_r_item.index_count = left_cyl_r_item.geometry->submesh("cylinder"_small)->index_count;
      left_cyl_r_item.start_index_location = left_cyl_r_item.geometry->submesh("cylinder"_small)->start_index_location;
      left_cyl_r_item.base_vertex_location = left_cyl_r_item.geometry->submesh("cylinder"_small)->base_vertex_location;
      left_cyl_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();

      right_cyl_r_item.world = left_cyl_world;
      right_cyl_r_item.constant_buffer_index = obj_cb_index++;
      right_cyl_r_item.geometry = m_meshes["scene_geometry"_med].get();
      right_cyl_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
      right_cyl_r_item.index_count = right_cyl_r_item.geometry->submesh("cylinder"_small)->index_count;
      right_cyl_r_item.start_index_location = right_cyl_r_item.geometry->submesh("cylinder"_small)->start_index_location;
      right_cyl_r_item.base_vertex_location = right_cyl_r_item.geometry->submesh("cylinder"_small)->base_vertex_location;
      right_cyl_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();

      left_sphere_r_item.world = left_sphere_world;
      left_sphere_r_item.constant_buffer_index = obj_cb_index++;
      left_sphere_r_item.geometry = m_meshes["scene_geometry"_med].get();
      left_sphere_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
      left_sphere_r_item.index_count = left_sphere_r_item.geometry->submesh("sphere"_small)->index_count;
      left_sphere_r_item.start_index_location = left_sphere_r_item.geometry->submesh("sphere"_small)->start_index_location;
      left_sphere_r_item.base_vertex_location = left_sphere_r_item.geometry->submesh("sphere"_small)->base_vertex_location;
      left_sphere_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();

      right_sphere_r_item.world = right_sphere_world;
      right_sphere_r_item.constant_buffer_index = obj_cb_index++;
      right_sphere_r_item.geometry = m_meshes["scene_geometry"_med].get();
      right_sphere_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
      right_sphere_r_item.index_count = right_sphere_r_item.geometry->submesh("sphere"_small)->index_count;
      right_sphere_r_item.start_index_location = right_sphere_r_item.geometry->submesh("sphere"_small)->start_index_location;
      right_sphere_r_item.base_vertex_location = right_sphere_r_item.geometry->submesh("sphere"_small)->base_vertex_location;
      right_sphere_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();

      add_render_item(rsl::move(left_cyl_r_item));
      add_render_item(rsl::move(right_cyl_r_item));
      add_render_item(rsl::move(left_sphere_r_item));
      add_render_item(rsl::move(right_sphere_r_item));
    }

  }

  void SampleScene::build_shader()
  {
    // Shader
    rex::renderer::commands::CompileShaderCommandDesc vs_compile_command_desc = rex::renderer::commands::create_compile_shader_parameters("standardVS"_small, rex::renderer::ShaderType::VERTEX, "regina\\Shaders\\color.hlsl");
    rex::renderer::commands::CompileShaderCommandDesc ps_compile_command_desc = rex::renderer::commands::create_compile_shader_parameters("opaquePS"_small, rex::renderer::ShaderType::PIXEL, "regina\\Shaders\\color.hlsl");

    rex::renderer::commands::LinkShaderCommandDesc link_shader_command_desc;
    link_shader_command_desc.vertex_shader = rex::renderer::compile_shader(rsl::move(vs_compile_command_desc));
    link_shader_command_desc.pixel_shader = rex::renderer::compile_shader(rsl::move(ps_compile_command_desc));
    link_shader_command_desc.constants = { rex::renderer::commands::ConstantLayoutDescription {rex::renderer::commands::ConstantType::CBuffer, "ObjectConstants", 0},
                                              rex::renderer::commands::ConstantLayoutDescription {rex::renderer::commands::ConstantType::CBuffer, "PassConstants", 1} };
    m_shader_program = rex::renderer::link_shader(rsl::move(link_shader_command_desc));

    // Input Layout
    rex::renderer::commands::CreateInputLayoutCommandDesc input_layout_command_desc;
    input_layout_command_desc.input_layout = { rex::renderer::commands::InputLayoutDescription {"POSITION", 0, rex::renderer::VertexBufferFormat::FLOAT3, 0, 0, rex::renderer::InputLayoutClassification::PerVertexData, 0},
                                              rex::renderer::commands::InputLayoutDescription {"COLOR", 0, rex::renderer::VertexBufferFormat::FLOAT4, 0, 12, rex::renderer::InputLayoutClassification::PerVertexData, 0} };
    m_input_layout = rex::renderer::create_input_layout(rsl::move(input_layout_command_desc));
  }

  void SampleScene::build_raster_state()
  {
    rex::renderer::commands::CreateRasterStateCommandDesc solid_rs_command_desc;
    solid_rs_command_desc.fill_mode = rex::renderer::FillMode::SOLID;
    solid_rs_command_desc.cull_mode = rex::renderer::CullMode::BACK;
    m_raster_state = rex::renderer::create_raster_state(rsl::move(solid_rs_command_desc));
  }

  void SampleScene::build_pso()
  {
    // Pipeline state object has to be aware of the render targets
    // This is because it needs to query the render target texture format
    // If there are no render targets set no PSO can be created
    rex::renderer::set_render_targets(rex::globals::default_targets_info().back_buffer_color, rex::globals::default_targets_info().depth_buffer);

    rex::renderer::commands::CreatePipelineStateCommandDesc create_pso_command_desc;
    create_pso_command_desc.input_layout = m_input_layout;
    create_pso_command_desc.num_render_targets = 1;
    create_pso_command_desc.shader_program = m_shader_program;
    create_pso_command_desc.rasterizer_state = m_raster_state;

    m_pso = rex::renderer::create_pipeline_state_object(rsl::move(create_pso_command_desc));
  }

  void SampleScene::build_frame_resources()
  {
    for (int i = 0; i < rex::renderer::max_frames_in_flight(); ++i)
    {
      rex::renderer::ResourceSlot frame_slot = rex::renderer::create_frame_resource();

      m_frame_resource_data.emplace_back(frame_slot);
    }
  }
  void SampleScene::build_constant_buffers()
  {
    const s32 num_render_items = render_item_count();

    // Attach committed resources required for each frame
    // The committed resources contain the uploader for each constant buffer that is created for a specific frame
    // The uploader needs to be kept alive at least until the commandlist is executed,
    //      hence why we store it in a separate resource so the resource pools keeps a reference to it
    // This internal knowledge should move to the renderer itself, as it can create the uploader for a CBV
    //      when a CBV allocation is requested.
    for (s32 frame = 0; frame < rex::renderer::max_frames_in_flight(); ++frame)
    {
      m_frame_resource_data[frame].set_object_committed_resource_slot(frame, num_render_items, sizeof(ObjectConstants));
      m_frame_resource_data[frame].set_pass_committed_resource_slot(frame, 1, sizeof(PassConstants));
    }

    // Need a CBV descriptor for each object for each frame resource.
    for (s32 frame = 0; frame < rex::renderer::max_frames_in_flight(); ++frame)
    {
      for (s32 i = 0; i < num_render_items; ++i)
      {
        m_frame_resource_data[frame].add_object_constant_buffer_slot(frame, m_frame_resource_data[frame].object_committed_resource_slot(), sizeof(ObjectConstants));
      }
    }

    // Last three descriptors are the pass CBVs for each frame resource.
    for (s32 frame = 0; frame < rex::renderer::max_frames_in_flight(); ++frame)
    {
      m_frame_resource_data[frame].set_pass_constant_buffer_slot(frame, m_frame_resource_data[frame].pass_committed_resource_slot(), sizeof(PassConstants));
    }

  }

  void SampleScene::update_view()
  {
    // Convert Spherical to Cartesian coordinates.
    m_eye_pos.x = 15.0f * sinf(0.2f * glm::pi<f32>()) * cosf(1.5f * glm::pi<f32>());
    m_eye_pos.y = 15.0f * cosf(0.2f * glm::pi<f32>());
    m_eye_pos.z = 35.0f * sinf(0.2f * glm::pi<f32>()) * sinf(1.5f * glm::pi<f32>());

    // Build the view matrix.
    const glm::vec3 pos = glm::vec3(m_eye_pos.x, m_eye_pos.y, m_eye_pos.z);
    const glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    m_view = glm::lookAt(pos, target, up);
    m_view = glm::transpose(m_view); // DirectX backend ( so we have to transpose, expects row major matrices )
  }
  void SampleScene::update_pass_constant_buffers()
  {
    rex::renderer::ResourceSlot const curr_pass_cr = get_pass_committed_resource_of_frame(rex::renderer::active_frame());

    const glm::mat4& view = m_view;
    const glm::mat4& proj = m_proj;

    const glm::mat4 view_proj = view * proj;

    const glm::mat4 inv_view = glm::inverse(view);
    const glm::mat4 inv_proj = glm::inverse(proj);
    const glm::mat4 inv_view_proj = glm::inverse(view_proj);

    m_pass_constants.view = view;
    m_pass_constants.inv_view = inv_view;
    m_pass_constants.proj = proj;
    m_pass_constants.inv_proj = inv_proj;
    m_pass_constants.view_proj = view_proj;
    m_pass_constants.inv_view_proj = inv_view_proj;

    const f32 window_width = static_cast<f32>(rex::globals::window_info().width);
    const f32 window_height = static_cast<f32>(rex::globals::window_info().height);

    m_pass_constants.eye_pos_w = m_eye_pos;
    m_pass_constants.render_target_size = glm::vec2(window_width, window_height);
    m_pass_constants.inv_render_target_size = glm::vec2(1.0f / window_width, 1.0f / window_height);
    m_pass_constants.near_z = rex::globals::default_depth_info().near_plane;
    m_pass_constants.far_z = rex::globals::default_depth_info().far_plane;
    m_pass_constants.delta_time = rex::globals::frame_info().delta_time().to_seconds();

    rex::renderer::commands::UpdateCommittedResourceCommandDesc update_constant_buffer_command_desc;
    update_constant_buffer_command_desc.element_index = 0;
    update_constant_buffer_command_desc.buffer_data = rex::memory::make_blob(reinterpret_cast<rsl::byte*>(&m_pass_constants), rsl::memory_size(sizeof(PassConstants)));

    rex::renderer::update_committed_resource(rsl::move(update_constant_buffer_command_desc), curr_pass_cr);
  }

  //-------------------------------------------------------------------------
  rex::renderer::ResourceSlot SampleScene::get_object_committed_resource_of_frame(const rex::renderer::ResourceSlot* frame)
  {
    auto it = rsl::find_if(m_frame_resource_data.cbegin(), m_frame_resource_data.cend(), [frame](const FrameData& data) { return *frame == data.frame_slot(); });

    return it != m_frame_resource_data.cend() ? it->object_committed_resource_slot() : rex::renderer::ResourceSlot::make_invalid();
  }
  //-------------------------------------------------------------------------
  rex::renderer::ResourceSlot SampleScene::get_pass_committed_resource_of_frame(const rex::renderer::ResourceSlot* frame)
  {
    auto it = rsl::find_if(m_frame_resource_data.cbegin(), m_frame_resource_data.cend(), [frame](const FrameData& data) { return *frame == data.frame_slot(); });

    return it != m_frame_resource_data.cend() ? it->pass_committed_resource_slot() : rex::renderer::ResourceSlot::make_invalid();
  }
  //-------------------------------------------------------------------------
  rsl::vector<rex::renderer::ResourceSlot> SampleScene::get_active_constant_buffer_for_frame(s32 frame)
  {
    auto it = rsl::find_if(m_frame_resource_data.cbegin(), m_frame_resource_data.cend(), [frame](const FrameData& data) { return frame == data.frame_slot().slot_id(); });
    return it != m_frame_resource_data.cend() ? it->object_constant_buffer_slots() : rsl::vector<rex::renderer::ResourceSlot>{};
  }
  //-------------------------------------------------------------------------
  rex::renderer::ResourceSlot SampleScene::get_active_pass_constant_buffer_for_frame(s32 frame)
  {
    auto it = rsl::find_if(m_frame_resource_data.cbegin(), m_frame_resource_data.cend(), [frame](const FrameData& data) { return frame == data.frame_slot().slot_id(); });

    return it != m_frame_resource_data.cend() ? it->pass_constant_buffer_slot() : rex::renderer::ResourceSlot::make_invalid();
  }

}

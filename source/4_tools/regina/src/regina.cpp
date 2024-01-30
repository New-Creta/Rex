#include "rex_engine/app/core_application.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/engine/entrypoint.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_engine/primitives/box.h"
#include "rex_engine/primitives/cylinder.h"
#include "rex_engine/primitives/grid.h"
#include "rex_engine/primitives/mesh_factory.h"
#include "rex_engine/primitives/sphere.h"
#include "rex_engine/windowinfo.h"
#include "rex_renderer_core/commands/attach_committed_resource_to_frame_cmd.h"
#include "rex_renderer_core/commands/compile_shader_cmd.h"
#include "rex_renderer_core/commands/create_buffer_cmd.h"
#include "rex_renderer_core/commands/create_clear_state_cmd.h"
#include "rex_renderer_core/commands/create_constant_buffer_cmd.h"
#include "rex_renderer_core/commands/create_constant_layout_description_cmd.h"
#include "rex_renderer_core/commands/create_input_layout_cmd.h"
#include "rex_renderer_core/commands/create_pipeline_state_cmd.h"
#include "rex_renderer_core/commands/create_raster_state_cmd.h"
#include "rex_renderer_core/commands/link_shader_cmd.h"
#include "rex_renderer_core/commands/update_committed_resource_cmd.h"
#include "rex_renderer_core/default_depth_info.h"
#include "rex_renderer_core/default_targets_info.h"
#include "rex_renderer_core/renderer.h"
#include "rex_renderer_core/rendering/scene.h"
#include "rex_renderer_core/rendering/scene_renderer.h"
#include "rex_renderer_core/resources/mesh.h"
#include "rex_renderer_core/resources/vertex.h"
#include "rex_renderer_core/scissor_rect.h"
#include "rex_renderer_core/viewport.h"

#include "rex_std/string.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/bonus/math/color.h"

#include "rex_windows/gui_application.h"
#include "rex_windows/platform_creation_params.h"

#include <glm/gtc/matrix_transform.hpp>

DEFINE_LOG_CATEGORY(LogRegina, rex::LogVerbosity::Log);

#define RENDER_WIREFRAME 0
#define RENDER_SCENE     1

namespace regina
{
  struct ObjectConstants
  {
    glm::mat4 world = glm::mat4(1.0f);
  };

  struct PassConstants
  {
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 inv_view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);
    glm::mat4 inv_proj = glm::mat4(1.0f);
    glm::mat4 view_proj = glm::mat4(1.0f);
    glm::mat4 inv_view_proj = glm::mat4(1.0f);

    glm::vec3 eye_pos_w = { 0.0f, 0.0f, 0.0f };
    f32 cb_padding_1 = 0.0f;

    glm::vec2 render_target_size = { 0.0f, 0.0f };
    glm::vec2 inv_render_target_size = { 0.0f, 0.0f };

    f32 near_z = 0.0f;
    f32 far_z = 0.0f;

    f32 total_time = 0.0f;
    f32 delta_time = 0.0f;
  };

  class FrameData
  {
  public:
    //-------------------------------------------------------------------------
    FrameData(const rex::renderer::ResourceSlot& frameSlot)
      : m_frame_slot(frameSlot)
      , m_object_committed_resource_slot()
      , m_pass_committed_resource_slot()
      , m_object_constant_buffer_slots()
      , m_pass_constant_buffer_slot()
    {}

    //-------------------------------------------------------------------------
    FrameData(const FrameData& other) = delete;
    //-------------------------------------------------------------------------
    FrameData(FrameData&& other) noexcept = default;

    //-------------------------------------------------------------------------
    ~FrameData()
    {
      m_frame_slot.release();
      m_object_constant_buffer_slots.clear();
      m_pass_constant_buffer_slot.release();
    }

    //-------------------------------------------------------------------------
    FrameData& operator=(const FrameData& other) = delete;
    //-------------------------------------------------------------------------
    FrameData& operator=(FrameData&& other) noexcept = default;

    //-------------------------------------------------------------------------
    void set_object_committed_resource_slot(s32 frameIndex, s32 numItems, s32 bufferByteSize)
    {
      rex::renderer::commands::AttachCommittedResourceToFrameCommandDesc attach_object_constants{};
      attach_object_constants.frame_index = frameIndex;
      attach_object_constants.buffer_count = numItems;
      attach_object_constants.buffer_byte_size = bufferByteSize;

      m_object_committed_resource_slot = rex::renderer::attach_committed_resource_to_frame(rsl::move(attach_object_constants));
    }

    //-------------------------------------------------------------------------
    void set_pass_committed_resource_slot(s32 frameIndex, s32 numItems, s32 bufferByteSize)
    {
      rex::renderer::commands::AttachCommittedResourceToFrameCommandDesc attach_object_constants{};
      attach_object_constants.frame_index = frameIndex;
      attach_object_constants.buffer_count = numItems;
      attach_object_constants.buffer_byte_size = bufferByteSize;

      m_pass_committed_resource_slot = rex::renderer::attach_committed_resource_to_frame(rsl::move(attach_object_constants));
    }

    //-------------------------------------------------------------------------
    void add_object_constant_buffer_slot(s32 frameIndex, const rex::renderer::ResourceSlot& committedResourceSlot, s32 bufferSize)
    {
      rex::renderer::commands::CreateConstantBufferViewCommandDesc create_const_buffer_command_desc{};

      create_const_buffer_command_desc.frame_index = frameIndex;
      create_const_buffer_command_desc.committed_resource = committedResourceSlot;
      create_const_buffer_command_desc.buffer_size = bufferSize;

      rex::renderer::ResourceSlot object_constant_buffer = rex::renderer::create_constant_buffer_view(rsl::move(create_const_buffer_command_desc));

      m_object_constant_buffer_slots.push_back(object_constant_buffer);
    }

    //-------------------------------------------------------------------------
    void set_pass_constant_buffer_slot(s32 frameIndex, const rex::renderer::ResourceSlot& committedResourceSlot, s32 bufferSize)
    {
      rex::renderer::commands::CreateConstantBufferViewCommandDesc create_const_buffer_command_desc{};

      create_const_buffer_command_desc.frame_index = frameIndex;
      create_const_buffer_command_desc.committed_resource = committedResourceSlot;
      create_const_buffer_command_desc.buffer_size = bufferSize;

      rex::renderer::ResourceSlot pass_constant_buffer = rex::renderer::create_constant_buffer_view(rsl::move(create_const_buffer_command_desc));

      m_pass_constant_buffer_slot = rsl::move(pass_constant_buffer);
    }

    //-------------------------------------------------------------------------
    const rex::renderer::ResourceSlot& frame_slot() const
    {
      return m_frame_slot;
    }

    //-------------------------------------------------------------------------
    const rex::renderer::ResourceSlot& object_committed_resource_slot() const
    {
      return m_object_committed_resource_slot;
    }

    //-------------------------------------------------------------------------
    const rex::renderer::ResourceSlot& pass_committed_resource_slot() const
    {
      return m_pass_committed_resource_slot;
    }

    //-------------------------------------------------------------------------
    const rsl::vector<rex::renderer::ResourceSlot>& object_constant_buffer_slots() const
    {
      return m_object_constant_buffer_slots;
    }

    //-------------------------------------------------------------------------
    const rex::renderer::ResourceSlot& pass_constant_buffer_slot() const
    {
      return m_pass_constant_buffer_slot;
    }

  private:
    rex::renderer::ResourceSlot m_frame_slot;
    rex::renderer::ResourceSlot m_object_committed_resource_slot;
    rex::renderer::ResourceSlot m_pass_committed_resource_slot;

    rsl::vector<rex::renderer::ResourceSlot> m_object_constant_buffer_slots;
    rex::renderer::ResourceSlot m_pass_constant_buffer_slot;
  };

  struct ReginaContext
  {
    rsl::vector<FrameData> frame_resource_data;

    rsl::unordered_map<rsl::medium_stack_string, rsl::unique_ptr<rex::renderer::Mesh>> meshes;

    rsl::unique_ptr<rex::renderer::Mesh> mesh_cube;
    rsl::unique_ptr<rex::renderer::Scene> scene;
    rsl::unique_ptr<rex::renderer::SceneRenderer> scene_renderer;

    rex::renderer::ResourceSlot clear_state;
    rex::renderer::ResourceSlot shader_program;
    rex::renderer::ResourceSlot input_layout;
    rex::renderer::ResourceSlot pso;
    rex::renderer::ResourceSlot solid_raster_state;
    rex::renderer::ResourceSlot wire_raster_state;

    PassConstants pass_constants;

    bool is_wireframe = false;

    glm::vec3 eye_pos = { 0.0f, 0.0f, 0.0f };
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);
  };

  ReginaContext g_regina_ctx; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

  //-------------------------------------------------------------------------
  rex::renderer::ResourceSlot get_object_committed_resource_of_frame(const rex::renderer::ResourceSlot* frame)
  {
    auto it = rsl::find_if(rsl::cbegin(g_regina_ctx.frame_resource_data), rsl::cend(g_regina_ctx.frame_resource_data), [frame](const FrameData& data) { return *frame == data.frame_slot(); });

    return it != rsl::cend(g_regina_ctx.frame_resource_data) ? it->object_committed_resource_slot() : rex::renderer::ResourceSlot::make_invalid();
  }
  //-------------------------------------------------------------------------
  rex::renderer::ResourceSlot get_pass_committed_resource_of_frame(const rex::renderer::ResourceSlot* frame)
  {
    auto it = rsl::find_if(rsl::cbegin(g_regina_ctx.frame_resource_data), rsl::cend(g_regina_ctx.frame_resource_data), [frame](const FrameData& data) { return *frame == data.frame_slot(); });

    return it != rsl::cend(g_regina_ctx.frame_resource_data) ? it->pass_committed_resource_slot() : rex::renderer::ResourceSlot::make_invalid();
  }
  //-------------------------------------------------------------------------
  rex::renderer::ResourceSlot get_active_object_constant_buffer_for_frame(s32 frame, s32 idx)
  {
    auto it = rsl::find_if(rsl::cbegin(g_regina_ctx.frame_resource_data), rsl::cend(g_regina_ctx.frame_resource_data), [frame](const FrameData& data) { return frame == data.frame_slot().slot_id(); });

    return it != rsl::cend(g_regina_ctx.frame_resource_data) ? it->object_constant_buffer_slots()[idx] : rex::renderer::ResourceSlot::make_invalid();
  }
  //-------------------------------------------------------------------------
  rex::renderer::ResourceSlot get_active_pass_constant_buffer_for_frame(s32 frame)
  {
    auto it = rsl::find_if(rsl::cbegin(g_regina_ctx.frame_resource_data), rsl::cend(g_regina_ctx.frame_resource_data), [frame](const FrameData& data) { return frame == data.frame_slot().slot_id(); });

    return it != rsl::cend(g_regina_ctx.frame_resource_data) ? it->pass_constant_buffer_slot() : rex::renderer::ResourceSlot::make_invalid();
  }

  //-------------------------------------------------------------------------
  rex::renderer::commands::CompileShaderCommandDesc create_compile_shader_parameters(const rsl::small_stack_string& shaderName, rex::renderer::ShaderType shaderType, rsl::string_view filePath)
  {
    rex::renderer::commands::CompileShaderCommandDesc compile_shader_command_desc{};

    switch (shaderType)
    {
    case rex::renderer::ShaderType::VERTEX:
      compile_shader_command_desc.shader_entry_point = "VS";
      compile_shader_command_desc.shader_feature_target = "vs_5_1";
      break;
    case rex::renderer::ShaderType::PIXEL:
      compile_shader_command_desc.shader_entry_point = "PS";
      compile_shader_command_desc.shader_feature_target = "ps_5_1";
      break;
    default:
      break;
    }

    compile_shader_command_desc.shader_code = rex::vfs::read_file(filePath);
    compile_shader_command_desc.shader_name = shaderName;
    compile_shader_command_desc.shader_type = shaderType;

    return compile_shader_command_desc;
  }

  //-------------------------------------------------------------------------
  template <typename T>
  rex::renderer::commands::CreateBufferCommandDesc create_buffer_parameters(T* data, s32 num)
  {
    rex::renderer::commands::CreateBufferCommandDesc create_buffer_command_desc{};

    create_buffer_command_desc.buffer_data = rex::memory::make_blob<T>(data, num);

    return create_buffer_command_desc;
  }

  //-------------------------------------------------------------------------
  bool build_clear_state()
  {
    rex::renderer::commands::CreateClearStateCommandDesc create_clear_state_command_desc{};

    create_clear_state_command_desc.rgba = rsl::colors::LightSteelBlue;
    create_clear_state_command_desc.depth = 1.0f;
    create_clear_state_command_desc.stencil = 0x00;

    rex::StateController<rex::renderer::ClearBits> clear_flags;
    clear_flags.add_state(rex::renderer::ClearBits::CLEAR_COLOR_BUFFER);
    clear_flags.add_state(rex::renderer::ClearBits::CLEAR_DEPTH_BUFFER);
    clear_flags.add_state(rex::renderer::ClearBits::CLEAR_STENCIL_BUFFER);

    create_clear_state_command_desc.flags = clear_flags;

    g_regina_ctx.clear_state = rex::renderer::create_clear_state(rsl::move(create_clear_state_command_desc));

    return true;
  }

  //-------------------------------------------------------------------------
  bool build_shader_and_input_layout()
  {
    // Shader
    rex::renderer::commands::CompileShaderCommandDesc vs_compile_command_desc = create_compile_shader_parameters("standardVS"_small, rex::renderer::ShaderType::VERTEX, "Shaders\\color.hlsl");
    rex::renderer::commands::CompileShaderCommandDesc ps_compile_command_desc = create_compile_shader_parameters("opaquePS"_small, rex::renderer::ShaderType::PIXEL, "Shaders\\color.hlsl");

    rex::renderer::commands::LinkShaderCommandDesc link_shader_command_desc;
    link_shader_command_desc.vertex_shader = rex::renderer::compile_shader(rsl::move(vs_compile_command_desc));
    link_shader_command_desc.pixel_shader = rex::renderer::compile_shader(rsl::move(ps_compile_command_desc));
    link_shader_command_desc.constants = { rex::renderer::commands::ConstantLayoutDescription {rex::renderer::commands::ConstantType::CBUFFER, "ObjectConstants", 0},
                                              rex::renderer::commands::ConstantLayoutDescription {rex::renderer::commands::ConstantType::CBUFFER, "PassConstants", 1} };
    g_regina_ctx.shader_program = rex::renderer::link_shader(rsl::move(link_shader_command_desc));

    // Input Layout
    rex::renderer::commands::CreateInputLayoutCommandDesc input_layout_command_desc;
    input_layout_command_desc.input_layout = { rex::renderer::commands::InputLayoutDescription {"POSITION", 0, rex::renderer::VertexBufferFormat::FLOAT3, 0, 0, rex::renderer::InputLayoutClassification::PER_VERTEX_DATA, 0},
                                              rex::renderer::commands::InputLayoutDescription {"COLOR", 0, rex::renderer::VertexBufferFormat::FLOAT4, 0, 12, rex::renderer::InputLayoutClassification::PER_VERTEX_DATA, 0} };
    g_regina_ctx.input_layout = rex::renderer::create_input_layout(rsl::move(input_layout_command_desc));

    return true;
  }

  //-------------------------------------------------------------------------
  bool build_cube_geometry()
  {
    auto box = rex::mesh_factory::create_box(1.5f, 0.5f, 1.5f, 0);

    auto total_vertex_count = box.vertices().size();
    auto total_index_count = box.indices().size();

    rsl::vector<rex::renderer::VertexPosCol> box_vertices((rsl::Capacity)total_vertex_count);
    for (const rex::mesh_factory::Vertex& v : box.vertices())
    {
      rex::renderer::VertexPosCol nv({ v.position.x, v.position.y, v.position.z }, { v.position.x, v.position.y, v.position.z, 1.0f });
      box_vertices.push_back(nv);
    }

    rsl::vector<u16> box_indices((rsl::Capacity)total_index_count);
    box_indices.insert(box_indices.end(), rsl::begin(box.indices()), rsl::end(box.indices()));

    const u32 vb_byte_size = total_vertex_count * sizeof(rex::renderer::VertexPosCol);
    const u32 ib_byte_size = total_index_count * sizeof(u16);

    rex::renderer::commands::CreateBufferCommandDesc v_create_buffer_command_desc = create_buffer_parameters<rex::renderer::VertexPosCol>(box_vertices.data(), box_vertices.size());
    rex::renderer::Mesh::VertexBufferDesc vbd(
      rex::renderer::create_vertex_buffer(rsl::move(v_create_buffer_command_desc)),
      sizeof(rex::renderer::VertexPosCol),
      vb_byte_size);

    rex::renderer::commands::CreateBufferCommandDesc i_create_buffer_command_desc = create_buffer_parameters<u16>(box_indices.data(), box_indices.size());
    rex::renderer::Mesh::IndexBufferDesc ibd(rex::renderer::create_index_buffer(rsl::move(i_create_buffer_command_desc)),
      rex::renderer::IndexBufferFormat::R16_UINT,
      ib_byte_size);

    g_regina_ctx.mesh_cube = rsl::make_unique<rex::renderer::Mesh>("box_geometry"_med, vbd, ibd);
    g_regina_ctx.mesh_cube->add_submesh("box"_small, total_index_count, 0, 0);

    return true;
  }

  //-------------------------------------------------------------------------
  bool build_scene_geometry()
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
    u32 box_vertex_offset = 0;
    u32 grid_vertex_offset = box.vertices().size();
    u32 sphere_verteX_offset = grid_vertex_offset + grid.vertices().size();
    u32 cylinder_vertex_offset = sphere_verteX_offset + sphere.vertices().size();

    // Cache the starting index for each object in the concatenated index buffer.
    u32 box_index_offset = 0;
    u32 grid_index_offset = box.indices().size();
    u32 sphere_index_offset = grid_index_offset + grid.indices().size();
    u32 cylinder_index_offset = sphere_index_offset + sphere.indices().size();

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
    sphere_submesh.base_vertex_location = sphere_verteX_offset;

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

    rsl::vector<rex::renderer::VertexPosCol> vertices((rsl::Size)total_vertex_count);

    u32 k = 0;
    for (s32 i = 0; i < box.vertices().size(); ++i, ++k)
    {
      glm::vec3 position = box.vertices()[i].position;
      auto c = rsl::colors::OrangeRed;
      glm::vec4 color = { c.red, c.green, c.blue, c.alpha };

      vertices[k] = rex::renderer::VertexPosCol(position, color);
    }

    for (s32 i = 0; i < grid.vertices().size(); ++i, ++k)
    {
      glm::vec3 position = grid.vertices()[i].position;
      auto c = rsl::colors::ForestGreen;
      glm::vec4 color = { c.red, c.green, c.blue, c.alpha };

      vertices[k] = rex::renderer::VertexPosCol(position, color);
    }

    for (s32 i = 0; i < sphere.vertices().size(); ++i, ++k)
    {
      glm::vec3 position = sphere.vertices()[i].position;
      auto c = rsl::colors::Crimson;
      glm::vec4 color = { c.red, c.green, c.blue, c.alpha };

      vertices[k] = rex::renderer::VertexPosCol(position, color);
    }

    for (s32 i = 0; i < cylinder.vertices().size(); ++i, ++k)
    {
      glm::vec3 position = cylinder.vertices()[i].position;
      auto c = rsl::colors::SteelBlue;
      glm::vec4 color = { c.red, c.green, c.blue, c.alpha };

      vertices[k] = rex::renderer::VertexPosCol(position, color);
    }

    rsl::vector<u16> indices;
    indices.insert(indices.end(), rsl::begin(box.indices()), rsl::end(box.indices()));
    indices.insert(indices.end(), rsl::begin(grid.indices()), rsl::end(grid.indices()));
    indices.insert(indices.end(), rsl::begin(sphere.indices()), rsl::end(sphere.indices()));
    indices.insert(indices.end(), rsl::begin(cylinder.indices()), rsl::end(cylinder.indices()));

    const u32 vb_byte_size = total_vertex_count * sizeof(rex::renderer::VertexPosCol);
    const u32 ib_byte_size = total_index_count * sizeof(u16);

    rex::renderer::commands::CreateBufferCommandDesc v_create_buffer_command_desc = create_buffer_parameters<rex::renderer::VertexPosCol>(vertices.data(), vertices.size());
    rex::renderer::Mesh::VertexBufferDesc vbd(
      rex::renderer::create_vertex_buffer(rsl::move(v_create_buffer_command_desc)),
      sizeof(rex::renderer::VertexPosCol),
      vb_byte_size);

    rex::renderer::commands::CreateBufferCommandDesc i_create_buffer_command_desc = create_buffer_parameters<u16>(indices.data(), indices.size());
    rex::renderer::Mesh::IndexBufferDesc ibd(
      rex::renderer::create_index_buffer(rsl::move(i_create_buffer_command_desc)),
      rex::renderer::IndexBufferFormat::R16_UINT,
      ib_byte_size);

    auto geometry = rsl::make_unique<rex::renderer::Mesh>("scene_geometry"_med, vbd, ibd);

    geometry->add_submesh("box"_small, box_submesh);
    geometry->add_submesh("grid"_small, grid_submesh);
    geometry->add_submesh("sphere"_small, sphere_submesh);
    geometry->add_submesh("cylinder"_small, cylinder_submesh);

    g_regina_ctx.meshes[geometry->name()] = rsl::move(geometry);

    return true;
  }

  //-------------------------------------------------------------------------
  bool build_cube_render_items()
  {
    auto cube_r_item = rex::renderer::RenderItem();

    glm::mat4 scale = glm::scale(cube_r_item.world, glm::vec3(2.0f, 2.0f, 2.0f));

    cube_r_item.world = scale;
    cube_r_item.constant_buffer_index = 0;
    cube_r_item.geometry = g_regina_ctx.mesh_cube.get();
    cube_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
    cube_r_item.index_count = cube_r_item.geometry->submesh("box"_small)->index_count;
    cube_r_item.start_index_location = cube_r_item.geometry->submesh("box"_small)->start_index_location;
    cube_r_item.base_vertex_location = cube_r_item.geometry->submesh("box"_small)->base_vertex_location;

    // Dirty flag indicating the object data has changed and we need to update the constant buffer.
    // Because we have an object cbuffer for each FrameResource, we have to apply the
    // update to each FrameResource.
    //
    // Thus, when we modify object data we should set NumFramesDirty = gNumFrameResources
    // so that each frame resource gets the update.
    cube_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();

    g_regina_ctx.scene->add_render_item(rsl::move(cube_r_item));

    return true;
  }

  //-------------------------------------------------------------------------
  bool build_scene_render_items()
  {
    glm::mat4 box_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
    glm::mat4 box_scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));

    auto box_r_item = rex::renderer::RenderItem();
    box_r_item.world = box_trans * box_scale;
    box_r_item.constant_buffer_index = 0;
    box_r_item.geometry = g_regina_ctx.meshes["scene_geometry"_med].get();
    box_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
    box_r_item.index_count = box_r_item.geometry->submesh("box"_small)->index_count;
    box_r_item.start_index_location = box_r_item.geometry->submesh("box"_small)->start_index_location;
    box_r_item.base_vertex_location = box_r_item.geometry->submesh("box"_small)->base_vertex_location;
    box_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();
    g_regina_ctx.scene->add_render_item(rsl::move(box_r_item));

    auto grid_r_item = rex::renderer::RenderItem();
    grid_r_item.world = glm::mat4(1.0f);
    grid_r_item.constant_buffer_index = 1;
    grid_r_item.geometry = g_regina_ctx.meshes["scene_geometry"_med].get();
    grid_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
    grid_r_item.index_count = grid_r_item.geometry->submesh("grid"_small)->index_count;
    grid_r_item.start_index_location = grid_r_item.geometry->submesh("grid"_small)->start_index_location;
    grid_r_item.base_vertex_location = grid_r_item.geometry->submesh("grid"_small)->base_vertex_location;
    grid_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();
    g_regina_ctx.scene->add_render_item(rsl::move(grid_r_item));

    u32 obj_cb_index = 2;
    for (int i = 0; i < 5; ++i)
    {
      auto left_cyl_r_item = rex::renderer::RenderItem();
      auto right_cyl_r_item = rex::renderer::RenderItem();
      auto left_sphere_r_item = rex::renderer::RenderItem();
      auto right_sphere_r_item = rex::renderer::RenderItem();

      glm::mat4 left_cyl_world = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 1.5f, -10.0f + i * 5.0f));
      glm::mat4 right_cyl_world = glm::translate(glm::mat4(1.0f), glm::vec3(+5.0f, 1.5f, -10.0f + i * 5.0f));
      glm::mat4 left_sphere_world = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 3.5f, -10.0f + i * 5.0f));
      glm::mat4 right_sphere_world = glm::translate(glm::mat4(1.0f), glm::vec3(+5.0f, 3.5f, -10.0f + i * 5.0f));

      left_cyl_r_item.world = right_cyl_world;
      left_cyl_r_item.constant_buffer_index = obj_cb_index++;
      left_cyl_r_item.geometry = g_regina_ctx.meshes["scene_geometry"_med].get();
      left_cyl_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
      left_cyl_r_item.index_count = left_cyl_r_item.geometry->submesh("cylinder"_small)->index_count;
      left_cyl_r_item.start_index_location = left_cyl_r_item.geometry->submesh("cylinder"_small)->start_index_location;
      left_cyl_r_item.base_vertex_location = left_cyl_r_item.geometry->submesh("cylinder"_small)->base_vertex_location;
      left_cyl_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();

      right_cyl_r_item.world = left_cyl_world;
      right_cyl_r_item.constant_buffer_index = obj_cb_index++;
      right_cyl_r_item.geometry = g_regina_ctx.meshes["scene_geometry"_med].get();
      right_cyl_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
      right_cyl_r_item.index_count = right_cyl_r_item.geometry->submesh("cylinder"_small)->index_count;
      right_cyl_r_item.start_index_location = right_cyl_r_item.geometry->submesh("cylinder"_small)->start_index_location;
      right_cyl_r_item.base_vertex_location = right_cyl_r_item.geometry->submesh("cylinder"_small)->base_vertex_location;
      right_cyl_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();

      left_sphere_r_item.world = left_sphere_world;
      left_sphere_r_item.constant_buffer_index = obj_cb_index++;
      left_sphere_r_item.geometry = g_regina_ctx.meshes["scene_geometry"_med].get();
      left_sphere_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
      left_sphere_r_item.index_count = left_sphere_r_item.geometry->submesh("sphere"_small)->index_count;
      left_sphere_r_item.start_index_location = left_sphere_r_item.geometry->submesh("sphere"_small)->start_index_location;
      left_sphere_r_item.base_vertex_location = left_sphere_r_item.geometry->submesh("sphere"_small)->base_vertex_location;
      left_sphere_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();

      right_sphere_r_item.world = right_sphere_world;
      right_sphere_r_item.constant_buffer_index = obj_cb_index++;
      right_sphere_r_item.geometry = g_regina_ctx.meshes["scene_geometry"_med].get();
      right_sphere_r_item.topology = rex::renderer::PrimitiveTopology::TRIANGLELIST;
      right_sphere_r_item.index_count = right_sphere_r_item.geometry->submesh("sphere"_small)->index_count;
      right_sphere_r_item.start_index_location = right_sphere_r_item.geometry->submesh("sphere"_small)->start_index_location;
      right_sphere_r_item.base_vertex_location = right_sphere_r_item.geometry->submesh("sphere"_small)->base_vertex_location;
      right_sphere_r_item.num_frames_dirty = rex::renderer::max_frames_in_flight();

      g_regina_ctx.scene->add_render_item(rsl::move(left_cyl_r_item));
      g_regina_ctx.scene->add_render_item(rsl::move(right_cyl_r_item));
      g_regina_ctx.scene->add_render_item(rsl::move(left_sphere_r_item));
      g_regina_ctx.scene->add_render_item(rsl::move(right_sphere_r_item));
    }

    return true;
  }

  //-------------------------------------------------------------------------
  bool build_frame_resources()
  {
    for (int i = 0; i < rex::renderer::max_frames_in_flight(); ++i)
    {
      rex::renderer::ResourceSlot frame_slot = rex::renderer::create_frame_resource();

      g_regina_ctx.frame_resource_data.emplace_back(frame_slot);
    }

    return true;
  }

  //-------------------------------------------------------------------------
  bool build_constant_buffers()
  {
    s32 num_render_items = g_regina_ctx.scene->render_item_count();

    // Attach committed resources required for each frame
    // The committed resources contain the uploader for each constant buffer that is created for a specific frame
    // The uploader needs to be kept alive at least until the commandlist is executed,
    //      hence why we store it in a separate resource so the resource pools keeps a reference to it
    // This internal knowledge should move to the renderer itself, as it can create the uploader for a CBV
    //      when a CBV allocation is requested.
    for (s32 frame = 0; frame < rex::renderer::max_frames_in_flight(); ++frame)
    {
      g_regina_ctx.frame_resource_data[frame].set_object_committed_resource_slot(frame, num_render_items, sizeof(ObjectConstants));
      g_regina_ctx.frame_resource_data[frame].set_pass_committed_resource_slot(frame, 1, sizeof(PassConstants));
    }

    // Need a CBV descriptor for each object for each frame resource.
    for (s32 frame = 0; frame < rex::renderer::max_frames_in_flight(); ++frame)
    {
      for (s32 i = 0; i < num_render_items; ++i)
      {
        g_regina_ctx.frame_resource_data[frame].add_object_constant_buffer_slot(frame, g_regina_ctx.frame_resource_data[frame].object_committed_resource_slot(), sizeof(ObjectConstants));
      }
    }

    // Last three descriptors are the pass CBVs for each frame resource.
    for (s32 frame = 0; frame < rex::renderer::max_frames_in_flight(); ++frame)
    {
      g_regina_ctx.frame_resource_data[frame].set_pass_constant_buffer_slot(frame, g_regina_ctx.frame_resource_data[frame].pass_committed_resource_slot(), sizeof(PassConstants));
    }

    return true;
  }

  //-------------------------------------------------------------------------
  bool build_raster_state()
  {
    rex::renderer::commands::CreateRasterStateCommandDesc solid_rs_command_desc;
    solid_rs_command_desc.fill_mode = rex::renderer::FillMode::SOLID;
    solid_rs_command_desc.cull_mode = rex::renderer::CullMode::BACK;
    g_regina_ctx.solid_raster_state = rex::renderer::create_raster_state(rsl::move(solid_rs_command_desc));

    rex::renderer::commands::CreateRasterStateCommandDesc wire_rs_command_desc;
    wire_rs_command_desc.fill_mode = rex::renderer::FillMode::WIREFRAME;
    wire_rs_command_desc.cull_mode = rex::renderer::CullMode::BACK;
    g_regina_ctx.wire_raster_state = rex::renderer::create_raster_state(rsl::move(wire_rs_command_desc));

    return true;
  }

  //-------------------------------------------------------------------------
  bool build_pipeline_state_object()
  {
    // Pipeline state object has to be aware of the render targets
    // This is because it needs to query the render target texture format
    // If there are no render targets set no PSO can be created
    rex::renderer::set_render_targets(rex::globals::default_targets_info().back_buffer_color, rex::globals::default_targets_info().depth_buffer);

    rex::renderer::commands::CreatePipelineStateCommandDesc create_pso_command_desc;
    create_pso_command_desc.input_layout = g_regina_ctx.input_layout;
    create_pso_command_desc.num_render_targets = 1;
    create_pso_command_desc.shader_program = g_regina_ctx.shader_program;
#if RENDER_WIREFRAME
    create_pso_command_desc.rasterizer_state = g_regina_ctx.wire_raster_state;
#else
    create_pso_command_desc.rasterizer_state = g_regina_ctx.solid_raster_state;
#endif
    g_regina_ctx.pso = rex::renderer::create_pipeline_state_object(rsl::move(create_pso_command_desc));

    return true;
  }

  //-------------------------------------------------------------------------
  void update_view()
  {
    // Convert Spherical to Cartesian coordinates.
    g_regina_ctx.eye_pos.x = 15.0f * sinf(0.2f * glm::pi<f32>()) * cosf(1.5f * glm::pi<f32>());
    g_regina_ctx.eye_pos.y = 15.0f * cosf(0.2f * glm::pi<f32>());
    g_regina_ctx.eye_pos.z = 35.0f * sinf(0.2f * glm::pi<f32>()) * sinf(1.5f * glm::pi<f32>());

    // Build the view matrix.
    glm::vec3 pos = glm::vec3(g_regina_ctx.eye_pos.x, g_regina_ctx.eye_pos.y, g_regina_ctx.eye_pos.z);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    g_regina_ctx.view = glm::lookAt(pos, target, up);
    g_regina_ctx.view = glm::transpose(g_regina_ctx.view); // DirectX backend ( so we have to transpose, expects row major matrices )
  }
  //-------------------------------------------------------------------------
  void update_object_constant_buffers()
  {
    for (auto& ri : (*g_regina_ctx.scene))
    {
      // Only update the cbuffer data if the constants have changed.
      // This needs to be tracked per frame resource.
      if (ri.num_frames_dirty > 0)
      {
        // Assign the new world matrixz
        ObjectConstants obj_constants;
        obj_constants.world = glm::transpose(ri.world); // DirectX backend ( so we have to transpose, expects row major matrices )

        rex::renderer::commands::UpdateCommittedResourceCommandDesc update_constant_buffer_command_desc;
        update_constant_buffer_command_desc.element_index = ri.constant_buffer_index;
        update_constant_buffer_command_desc.buffer_data = rex::memory::make_blob((rsl::byte*)&obj_constants, rsl::memory_size(sizeof(ObjectConstants)));

        rex::renderer::ResourceSlot curr_object_cr = get_object_committed_resource_of_frame(rex::renderer::active_frame());
        rex::renderer::update_committed_resource(rsl::move(update_constant_buffer_command_desc), curr_object_cr);
      }
    }
  }
  //-------------------------------------------------------------------------
  void update_pass_constant_buffers()
  {
    rex::renderer::ResourceSlot curr_pass_cr = get_pass_committed_resource_of_frame(rex::renderer::active_frame());

    const glm::mat4& view = g_regina_ctx.view;
    const glm::mat4& proj = g_regina_ctx.proj;

    glm::mat4 view_proj = view * proj;

    glm::mat4 inv_view = glm::inverse(view);
    glm::mat4 inv_proj = glm::inverse(proj);
    glm::mat4 inv_view_proj = glm::inverse(view_proj);

    g_regina_ctx.pass_constants.view = view;
    g_regina_ctx.pass_constants.inv_view = inv_view;
    g_regina_ctx.pass_constants.proj = proj;
    g_regina_ctx.pass_constants.inv_proj = inv_proj;
    g_regina_ctx.pass_constants.view_proj = view_proj;
    g_regina_ctx.pass_constants.inv_view_proj = inv_view_proj;

    f32 window_width = static_cast<f32>(rex::globals::window_info().width);
    f32 window_height = static_cast<f32>(rex::globals::window_info().height);

    g_regina_ctx.pass_constants.eye_pos_w = g_regina_ctx.eye_pos;
    g_regina_ctx.pass_constants.render_target_size = glm::vec2(window_width, window_height);
    g_regina_ctx.pass_constants.inv_render_target_size = glm::vec2(1.0f / window_width, 1.0f / window_height);
    g_regina_ctx.pass_constants.near_z = rex::globals::default_depth_info().near_plane;
    g_regina_ctx.pass_constants.far_z = rex::globals::default_depth_info().far_plane;
    g_regina_ctx.pass_constants.delta_time = rex::globals::frame_info().delta_time().to_seconds();

    rex::renderer::commands::UpdateCommittedResourceCommandDesc update_constant_buffer_command_desc;
    update_constant_buffer_command_desc.element_index = 0;
    update_constant_buffer_command_desc.buffer_data = rex::memory::make_blob((rsl::byte*)&g_regina_ctx.pass_constants, rsl::memory_size(sizeof(PassConstants)));

    rex::renderer::update_committed_resource(rsl::move(update_constant_buffer_command_desc), curr_pass_cr);
  }

  //-------------------------------------------------------------------------
  bool init_gfx()
  {
    g_regina_ctx.scene = rsl::make_unique<rex::renderer::Scene>();
    g_regina_ctx.scene_renderer = rsl::make_unique<rex::renderer::SceneRenderer>(g_regina_ctx.scene.get());

    if (!build_clear_state())
      return false;
    if (!build_shader_and_input_layout())
      return false;
#if RENDER_SCENE
    if (!build_scene_geometry())
      return false;
    if (!build_scene_render_items())
      return false;
#else
    if (!build_cube_geometry())
      return false;
    if (!build_cube_render_items())
      return false;
#endif
    if (!build_frame_resources())
      return false;
    if (!build_constant_buffers())
      return false;
    if (!build_raster_state())
      return false;
    if (!build_pipeline_state_object())
      return false;

    // The window resized, so update the aspect ratio and recompute the projection matrix.
    g_regina_ctx.proj = glm::perspectiveFov(0.25f * glm::pi<f32>(), static_cast<f32>(rex::globals::window_info().width), static_cast<f32>(rex::globals::window_info().height), rex::globals::default_depth_info().near_plane, rex::globals::default_depth_info().far_plane);
    g_regina_ctx.proj = glm::transpose(g_regina_ctx.proj); // DirectX backend ( so we have to transpose, expects row major matrices )

    return true;
  }

  //-------------------------------------------------------------------------
  bool initialize()
  {
    REX_LOG(LogRegina, "Initializing Regina");

    if (!init_gfx())
      return false;

    return true;
  }
  //-------------------------------------------------------------------------
  void update()
  {
    update_view();

    // Has the GPU finished processing the commands of the current frame resource?
    // If not, wait until the GPU has completed commands up to this fence point.
    //
    // This call is not queued but directly executed!
    rex::renderer::wait_for_active_frame();
  }
  //-------------------------------------------------------------------------
  void draw()
  {
    rex::Viewport vp = { 0.0f, 0.0f, (f32)rex::globals::window_info().width, (f32)rex::globals::window_info().height, 0.0f, 1.0f };
    rex::ScissorRect sr = { vp.top_left_x, vp.top_left_y, vp.width, vp.height };

    rex::renderer::set_pipeline_state_object(g_regina_ctx.pso);
    rex::renderer::set_raster_state(g_regina_ctx.solid_raster_state);

    rex::renderer::new_frame();

    update_object_constant_buffers();
    update_pass_constant_buffers();

    rex::renderer::set_render_targets(rex::globals::default_targets_info().back_buffer_color, rex::globals::default_targets_info().depth_buffer);
    rex::renderer::set_viewport(vp);
    rex::renderer::set_scissor_rect(sr);

    rex::renderer::begin_draw();

    rex::renderer::clear(g_regina_ctx.clear_state);

    rex::renderer::set_shader(g_regina_ctx.shader_program);

    rex::renderer::ResourceSlot curr_pass_cb = get_active_pass_constant_buffer_for_frame(rex::renderer::active_frame()->slot_id());
    rex::renderer::set_constant_buffer_view(curr_pass_cb, 1);

    for (auto& ri : (*g_regina_ctx.scene))
    {
      rex::renderer::set_vertex_buffer(ri.geometry->vertex_buffer_slot(), 0, ri.geometry->vertex_buffer_byte_stride(), 0);
      rex::renderer::set_index_buffer(ri.geometry->index_buffer_slot(), ri.geometry->index_buffer_format(), 0);
      rex::renderer::set_primitive_topology(rex::renderer::PrimitiveTopology::TRIANGLELIST);

      rex::renderer::ResourceSlot curr_object_cb = get_active_object_constant_buffer_for_frame(rex::renderer::active_frame()->slot_id(), ri.constant_buffer_index);
      rex::renderer::set_constant_buffer_view(curr_object_cb, 0);

      rex::renderer::renderer_draw_indexed_instanced(1, 0, ri.index_count, ri.start_index_location, ri.base_vertex_location);
    }

    rex::renderer::end_draw();

    rex::renderer::present();
    rex::renderer::end_frame();
  }
  //-------------------------------------------------------------------------
  void shutdown()
  {
    REX_LOG(LogRegina, "shutting down Regina");

    g_regina_ctx.clear_state.release();

    g_regina_ctx.shader_program.release();
    g_regina_ctx.input_layout.release();
    g_regina_ctx.pso.release();

    g_regina_ctx.frame_resource_data.clear();

    g_regina_ctx.solid_raster_state.release();
    g_regina_ctx.wire_raster_state.release();

    if (g_regina_ctx.mesh_cube)
    {
      g_regina_ctx.mesh_cube.reset();
    }
    else
    {
      g_regina_ctx.meshes.clear();
    }

    g_regina_ctx.mesh_cube.reset();
    g_regina_ctx.scene_renderer.reset();
    g_regina_ctx.scene.reset();
  }

  //-------------------------------------------------------------------------
  rex::ApplicationCreationParams create_regina_app_creation_params(rex::PlatformCreationParams&& platformParams)
  {
    rex::ApplicationCreationParams app_params(&platformParams);

    app_params.gui_params.window_width = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title = "Regina";

    app_params.engine_params.max_memory = 256_kb;
    app_params.engine_params.app_init_func = initialize;
    app_params.engine_params.app_update_func = update;
    app_params.engine_params.app_draw_func = draw;
    app_params.engine_params.app_shutdown_func = shutdown;
    app_params.create_window = true;

    return app_params;
  }

} // namespace regina

namespace rex
{
#ifndef REX_ENABLE_AUTO_TESTS
  //-------------------------------------------------------------------------
  ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams)
  {
    return regina::create_regina_app_creation_params(rsl::move(platformParams));
  }
#endif
} // namespace rex
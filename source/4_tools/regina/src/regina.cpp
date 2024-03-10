#include "rex_engine/app/core_application.h"
#include "rex_engine/cmdline/cmdline.h"
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
#include "rex_engine/app/windowinfo.h"
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
#include "rex_renderer_core/rendering/default_depth_info.h"
#include "rex_renderer_core/rendering/default_targets_info.h"
#include "rex_renderer_core/rendering/renderer.h"
#include "rex_renderer_core/rendering/scene.h"
#include "rex_renderer_core/rendering/scene_renderer.h"
#include "rex_renderer_core/resources/mesh.h"
#include "rex_renderer_core/resources/vertex.h"
#include "rex_renderer_core/rendering/scissor_rect.h"
#include "rex_renderer_core/rendering/viewport.h"
#include "rex_std/bonus/math/color.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/string.h"
#include "rex_windows/app/gui_application.h"
#include "rex_windows/engine/platform_creation_params.h"

#include <glm/gtc/matrix_transform.hpp>

#include "regina/sample_scene.h"
#include "regina/cube_scene.h"

DEFINE_LOG_CATEGORY(LogRegina, rex::LogVerbosity::Log);

namespace regina
{
#pragma region Graphics
  struct ObjectConstants
  {
    glm::mat4 world = glm::mat4(1.0f);
  };

  struct PassConstants
  {
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 inv_view      = glm::mat4(1.0f);
    glm::mat4 proj          = glm::mat4(1.0f);
    glm::mat4 inv_proj      = glm::mat4(1.0f);
    glm::mat4 view_proj     = glm::mat4(1.0f);
    glm::mat4 inv_view_proj = glm::mat4(1.0f);

    glm::vec3 eye_pos_w = {0.0f, 0.0f, 0.0f};
    f32 cb_padding_1    = 0.0f;

    glm::vec2 render_target_size     = {0.0f, 0.0f};
    glm::vec2 inv_render_target_size = {0.0f, 0.0f};

    f32 near_z = 0.0f;
    f32 far_z  = 0.0f;

    f32 total_time = 0.0f;
    f32 delta_time = 0.0f;
  };

  class FrameData
  {
  public:
    //-------------------------------------------------------------------------
    explicit FrameData(const rex::renderer::ResourceSlot& frameSlot)
        : m_frame_slot(frameSlot)
        , m_object_committed_resource_slot()
        , m_pass_committed_resource_slot()
        , m_object_constant_buffer_slots()
        , m_pass_constant_buffer_slot()
    {
    }

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
      rex::renderer::commands::AttachCommittedResourceToFrameCommandDesc attach_object_constants {};
      attach_object_constants.frame_index      = frameIndex;
      attach_object_constants.buffer_count     = numItems;
      attach_object_constants.buffer_byte_size = bufferByteSize;

      m_object_committed_resource_slot = rex::renderer::attach_committed_resource_to_frame(rsl::move(attach_object_constants));
    }

    //-------------------------------------------------------------------------
    void set_pass_committed_resource_slot(s32 frameIndex, s32 numItems, s32 bufferByteSize)
    {
      rex::renderer::commands::AttachCommittedResourceToFrameCommandDesc attach_object_constants {};
      attach_object_constants.frame_index      = frameIndex;
      attach_object_constants.buffer_count     = numItems;
      attach_object_constants.buffer_byte_size = bufferByteSize;

      m_pass_committed_resource_slot = rex::renderer::attach_committed_resource_to_frame(rsl::move(attach_object_constants));
    }

    //-------------------------------------------------------------------------
    void add_object_constant_buffer_slot(s32 frameIndex, const rex::renderer::ResourceSlot& committedResourceSlot, s32 bufferSize)
    {
      rex::renderer::commands::CreateConstantBufferViewCommandDesc create_const_buffer_command_desc {};

      create_const_buffer_command_desc.frame_index        = frameIndex;
      create_const_buffer_command_desc.committed_resource = committedResourceSlot;
      create_const_buffer_command_desc.buffer_size        = bufferSize;

      rex::renderer::ResourceSlot const object_constant_buffer = rex::renderer::create_constant_buffer_view(rsl::move(create_const_buffer_command_desc));

      m_object_constant_buffer_slots.push_back(object_constant_buffer);
    }

    //-------------------------------------------------------------------------
    void set_pass_constant_buffer_slot(s32 frameIndex, const rex::renderer::ResourceSlot& committedResourceSlot, s32 bufferSize)
    {
      rex::renderer::commands::CreateConstantBufferViewCommandDesc create_const_buffer_command_desc {};

      create_const_buffer_command_desc.frame_index        = frameIndex;
      create_const_buffer_command_desc.committed_resource = committedResourceSlot;
      create_const_buffer_command_desc.buffer_size        = bufferSize;

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

    glm::vec3 eye_pos = {0.0f, 0.0f, 0.0f};
    glm::mat4 view    = glm::mat4(1.0f);
    glm::mat4 proj    = glm::mat4(1.0f);
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
    rex::renderer::commands::CompileShaderCommandDesc compile_shader_command_desc {};

    switch(shaderType)
    {
      case rex::renderer::ShaderType::VERTEX:
        compile_shader_command_desc.shader_entry_point    = "VS";
        compile_shader_command_desc.shader_feature_target = "vs_5_1";
        break;
      case rex::renderer::ShaderType::PIXEL:
        compile_shader_command_desc.shader_entry_point    = "PS";
        compile_shader_command_desc.shader_feature_target = "ps_5_1";
        break;
      default: break;
    }

    compile_shader_command_desc.shader_code = rex::vfs::read_file(filePath);
    compile_shader_command_desc.shader_name = shaderName;
    compile_shader_command_desc.shader_type = shaderType;

    return compile_shader_command_desc;
  }

  //-------------------------------------------------------------------------
  bool build_clear_state()
  {
    rex::renderer::commands::CreateClearStateCommandDesc create_clear_state_command_desc {};

    create_clear_state_command_desc.rgba    = rsl::colors::LightSteelBlue;
    create_clear_state_command_desc.depth   = 1.0f;
    create_clear_state_command_desc.stencil = 0x00;

    rex::StateController<rex::renderer::ClearBits> clear_flags;
    clear_flags.add_state(rex::renderer::ClearBits::ClearColorBuffer);
    clear_flags.add_state(rex::renderer::ClearBits::ClearDepthBuffer);
    clear_flags.add_state(rex::renderer::ClearBits::ClearStencilBuffer);

    create_clear_state_command_desc.flags = clear_flags;

    g_regina_ctx.clear_state = rex::renderer::create_clear_state(rsl::move(create_clear_state_command_desc));

    return true;
  }

  //-------------------------------------------------------------------------
  bool build_shader_and_input_layout()
  {
    // Shader
    rex::renderer::commands::CompileShaderCommandDesc vs_compile_command_desc = create_compile_shader_parameters("standardVS"_small, rex::renderer::ShaderType::VERTEX, "regina\\Shaders\\color.hlsl");
    rex::renderer::commands::CompileShaderCommandDesc ps_compile_command_desc = create_compile_shader_parameters("opaquePS"_small, rex::renderer::ShaderType::PIXEL, "regina\\Shaders\\color.hlsl");

    rex::renderer::commands::LinkShaderCommandDesc link_shader_command_desc;
    link_shader_command_desc.vertex_shader = rex::renderer::compile_shader(rsl::move(vs_compile_command_desc));
    link_shader_command_desc.pixel_shader  = rex::renderer::compile_shader(rsl::move(ps_compile_command_desc));
    link_shader_command_desc.constants     = {rex::renderer::commands::ConstantLayoutDescription {rex::renderer::commands::ConstantType::CBuffer, "ObjectConstants", 0},
                                              rex::renderer::commands::ConstantLayoutDescription {rex::renderer::commands::ConstantType::CBuffer, "PassConstants", 1}};
    g_regina_ctx.shader_program            = rex::renderer::link_shader(rsl::move(link_shader_command_desc));

    // Input Layout
    rex::renderer::commands::CreateInputLayoutCommandDesc input_layout_command_desc;
    input_layout_command_desc.input_layout = {rex::renderer::commands::InputLayoutDescription {"POSITION", 0, rex::renderer::VertexBufferFormat::FLOAT3, 0, 0, rex::renderer::InputLayoutClassification::PerVertexData, 0},
                                              rex::renderer::commands::InputLayoutDescription {"COLOR", 0, rex::renderer::VertexBufferFormat::FLOAT4, 0, 12, rex::renderer::InputLayoutClassification::PerVertexData, 0}};
    g_regina_ctx.input_layout              = rex::renderer::create_input_layout(rsl::move(input_layout_command_desc));

    return true;
  }

  //-------------------------------------------------------------------------
  bool build_frame_resources()
  {
    for(int i = 0; i < rex::renderer::max_frames_in_flight(); ++i)
    {
      rex::renderer::ResourceSlot frame_slot = rex::renderer::create_frame_resource();

      g_regina_ctx.frame_resource_data.emplace_back(frame_slot);
    }

    return true;
  }

  //-------------------------------------------------------------------------
  bool build_constant_buffers()
  {
    const s32 num_render_items = g_regina_ctx.scene->render_item_count();

    // Attach committed resources required for each frame
    // The committed resources contain the uploader for each constant buffer that is created for a specific frame
    // The uploader needs to be kept alive at least until the commandlist is executed,
    //      hence why we store it in a separate resource so the resource pools keeps a reference to it
    // This internal knowledge should move to the renderer itself, as it can create the uploader for a CBV
    //      when a CBV allocation is requested.
    for(s32 frame = 0; frame < rex::renderer::max_frames_in_flight(); ++frame)
    {
      g_regina_ctx.frame_resource_data[frame].set_object_committed_resource_slot(frame, num_render_items, sizeof(ObjectConstants));
      g_regina_ctx.frame_resource_data[frame].set_pass_committed_resource_slot(frame, 1, sizeof(PassConstants));
    }

    // Need a CBV descriptor for each object for each frame resource.
    for(s32 frame = 0; frame < rex::renderer::max_frames_in_flight(); ++frame)
    {
      for(s32 i = 0; i < num_render_items; ++i)
      {
        g_regina_ctx.frame_resource_data[frame].add_object_constant_buffer_slot(frame, g_regina_ctx.frame_resource_data[frame].object_committed_resource_slot(), sizeof(ObjectConstants));
      }
    }

    // Last three descriptors are the pass CBVs for each frame resource.
    for(s32 frame = 0; frame < rex::renderer::max_frames_in_flight(); ++frame)
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
    create_pso_command_desc.input_layout       = g_regina_ctx.input_layout;
    create_pso_command_desc.num_render_targets = 1;
    create_pso_command_desc.shader_program     = g_regina_ctx.shader_program;
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
    const glm::vec3 pos    = glm::vec3(g_regina_ctx.eye_pos.x, g_regina_ctx.eye_pos.y, g_regina_ctx.eye_pos.z);
    const glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    const glm::vec3 up     = glm::vec3(0.0f, 1.0f, 0.0f);

    g_regina_ctx.view = glm::lookAt(pos, target, up);
    g_regina_ctx.view = glm::transpose(g_regina_ctx.view); // DirectX backend ( so we have to transpose, expects row major matrices )
  }
  //-------------------------------------------------------------------------
  void update_object_constant_buffers()
  {
    for(auto& ri: (*g_regina_ctx.scene))
    {
      // Only update the CBuffer data if the constants have changed.
      // This needs to be tracked per frame resource.
      if(ri.num_frames_dirty > 0)
      {
        // Assign the new world matrixz
        ObjectConstants obj_constants;
        obj_constants.world = glm::transpose(ri.world); // DirectX backend ( so we have to transpose, expects row major matrices )

        rex::renderer::commands::UpdateCommittedResourceCommandDesc update_constant_buffer_command_desc;
        update_constant_buffer_command_desc.element_index = ri.constant_buffer_index;
        update_constant_buffer_command_desc.buffer_data   = rex::memory::make_blob(reinterpret_cast<rsl::byte*>(&obj_constants), rsl::memory_size(sizeof(ObjectConstants)));

        rex::renderer::ResourceSlot const curr_object_cr = get_object_committed_resource_of_frame(rex::renderer::active_frame());
        rex::renderer::update_committed_resource(rsl::move(update_constant_buffer_command_desc), curr_object_cr);
      }
    }
  }
  //-------------------------------------------------------------------------
  void update_pass_constant_buffers()
  {
    rex::renderer::ResourceSlot const curr_pass_cr = get_pass_committed_resource_of_frame(rex::renderer::active_frame());

    const glm::mat4& view = g_regina_ctx.view;
    const glm::mat4& proj = g_regina_ctx.proj;

    const glm::mat4 view_proj = view * proj;

    const glm::mat4 inv_view      = glm::inverse(view);
    const glm::mat4 inv_proj      = glm::inverse(proj);
    const glm::mat4 inv_view_proj = glm::inverse(view_proj);

    g_regina_ctx.pass_constants.view          = view;
    g_regina_ctx.pass_constants.inv_view      = inv_view;
    g_regina_ctx.pass_constants.proj          = proj;
    g_regina_ctx.pass_constants.inv_proj      = inv_proj;
    g_regina_ctx.pass_constants.view_proj     = view_proj;
    g_regina_ctx.pass_constants.inv_view_proj = inv_view_proj;

    const f32 window_width  = static_cast<f32>(rex::globals::window_info().width);
    const f32 window_height = static_cast<f32>(rex::globals::window_info().height);

    g_regina_ctx.pass_constants.eye_pos_w              = g_regina_ctx.eye_pos;
    g_regina_ctx.pass_constants.render_target_size     = glm::vec2(window_width, window_height);
    g_regina_ctx.pass_constants.inv_render_target_size = glm::vec2(1.0f / window_width, 1.0f / window_height);
    g_regina_ctx.pass_constants.near_z                 = rex::globals::default_depth_info().near_plane;
    g_regina_ctx.pass_constants.far_z                  = rex::globals::default_depth_info().far_plane;
    g_regina_ctx.pass_constants.delta_time             = rex::globals::frame_info().delta_time().to_seconds();

    rex::renderer::commands::UpdateCommittedResourceCommandDesc update_constant_buffer_command_desc;
    update_constant_buffer_command_desc.element_index = 0;
    update_constant_buffer_command_desc.buffer_data   = rex::memory::make_blob(reinterpret_cast<rsl::byte*>(&g_regina_ctx.pass_constants), rsl::memory_size(sizeof(PassConstants)));

    rex::renderer::update_committed_resource(rsl::move(update_constant_buffer_command_desc), curr_pass_cr);
  }

  //-------------------------------------------------------------------------
  bool init_gfx()
  {
    if (rex::cmdline::get_argument("UseCubeScene"))
    {
      g_regina_ctx.scene = rsl::make_unique<regina::CubeScene>();
    }
    else
    {
      g_regina_ctx.scene = rsl::make_unique<regina::SampleScene>();
    }
    g_regina_ctx.scene_renderer = rsl::make_unique<rex::renderer::SceneRenderer>(g_regina_ctx.scene.get());

    if(!build_clear_state())
      return false;
    if(!build_shader_and_input_layout())
      return false;

    if(!build_frame_resources())
      return false;
    if(!build_constant_buffers())
      return false;
    if(!build_raster_state())
      return false;
    if(!build_pipeline_state_object())
      return false;

    // The window resized, so update the aspect ratio and recompute the projection matrix.
    const auto fov = 0.25f * glm::pi<f32>();
    const auto width = static_cast<f32>(rex::globals::window_info().width);
    const auto height = static_cast<f32>(rex::globals::window_info().height);
    const auto near_plane = rex::globals::default_depth_info().near_plane;
    const auto far_plane = rex::globals::default_depth_info().far_plane;
    g_regina_ctx.proj = glm::perspectiveFov(fov, width, height, near_plane, far_plane);
    g_regina_ctx.proj = glm::transpose(g_regina_ctx.proj); // DirectX backend ( so we have to transpose, expects row major matrices )

    rex::renderer::set_pipeline_state_object(g_regina_ctx.pso);
    rex::renderer::set_raster_state(g_regina_ctx.solid_raster_state);

    return true;
  }
#pragma endregion

  //-------------------------------------------------------------------------
  bool initialize()
  {
    REX_LOG(LogRegina, "Initializing Regina");

    return init_gfx();
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
    const rex::Viewport vp    = {0.0f, 0.0f, static_cast<f32>(rex::globals::window_info().width), static_cast<f32>(rex::globals::window_info().height), 0.0f, 1.0f};
    const rex::ScissorRect sr = {vp.top_left_x, vp.top_left_y, vp.width, vp.height};

    update_object_constant_buffers();
    update_pass_constant_buffers();

    rex::renderer::set_viewport(vp);
    rex::renderer::set_scissor_rect(sr);

    rex::renderer::clear(g_regina_ctx.clear_state);

    rex::renderer::set_shader(g_regina_ctx.shader_program);

    rex::renderer::ResourceSlot const curr_pass_cb = get_active_pass_constant_buffer_for_frame(rex::renderer::active_frame()->slot_id());
    rex::renderer::set_constant_buffer_view(curr_pass_cb, 1);

    for(auto& ri: (*g_regina_ctx.scene))
    {
      rex::renderer::set_vertex_buffer(ri.geometry->vertex_buffer_slot(), 0, ri.geometry->vertex_buffer_byte_stride(), 0);
      rex::renderer::set_index_buffer(ri.geometry->index_buffer_slot(), ri.geometry->index_buffer_format(), 0);
      rex::renderer::set_primitive_topology(rex::renderer::PrimitiveTopology::TRIANGLELIST);

      rex::renderer::ResourceSlot const curr_object_cb = get_active_object_constant_buffer_for_frame(rex::renderer::active_frame()->slot_id(), ri.constant_buffer_index);
      rex::renderer::set_constant_buffer_view(curr_object_cb, 0);

      rex::renderer::renderer_draw_indexed_instanced(1, 0, ri.index_count, ri.start_index_location, ri.base_vertex_location);
    }
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

    g_regina_ctx.scene_renderer.reset();
    g_regina_ctx.scene.reset();
  }

  //-------------------------------------------------------------------------
  rex::ApplicationCreationParams create_regina_app_creation_params(rex::PlatformCreationParams&& platformParams)
  {
    rex::ApplicationCreationParams app_params(&platformParams);

    app_params.gui_params.window_width  = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title  = "Regina";

    app_params.engine_params.app_init_func     = initialize;
    app_params.engine_params.app_update_func   = update;
    app_params.engine_params.app_draw_func     = draw;
    app_params.engine_params.app_shutdown_func = shutdown;
    app_params.create_window                   = true;

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
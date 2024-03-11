#include "rex_renderer_auto_test/rex_renderer_directx_initialize_test.h"
#include "rex_engine/event_system/event_system.h"
#include "rex_engine/memory/blob.h"
#include "rex_engine/windowinfo.h"
#include "rex_windows/platform_creation_params.h"
#include "rex_windows/gui_application.h"
#include "rex_renderer_core/renderer.h"
#include "rex_renderer_core/commands/compile_shader_cmd.h"
#include "rex_renderer_core/commands/link_shader_cmd.h"
#include "rex_renderer_core/commands/create_clear_state_cmd.h"
#include "rex_renderer_core/commands/create_pipeline_state_cmd.h"
#include "rex_renderer_core/commands/create_raster_state_cmd.h"
#include "rex_renderer_core/commands/create_input_layout_cmd.h"
#include "rex_renderer_core/default_depth_info.h"
#include "rex_renderer_core/default_targets_info.h"
#include "rex_renderer_core/viewport.h"
#include "rex_renderer_core/scissor_rect.h"
#include "rex_renderer_resources/resource_slot.h"

namespace rex_renderer_directx_build_clear_state_test
{ 
  struct TestContext
  {
    rsl::vector<rex::renderer::ResourceSlot> frames;

    rex::renderer::ResourceSlot clear_state;
    rex::renderer::ResourceSlot shader_program;
    rex::renderer::ResourceSlot input_layout;
    rex::renderer::ResourceSlot pso;
    rex::renderer::ResourceSlot solid_raster_state;

    s32 frame_counter = 0;
  };

  TestContext g_test_ctx; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

  std::string g_color_shader = "                                        \n\
    struct VertexIn                                                     \n\
    {                                                                   \n\
        float3 PosL : POSITION;                                         \n\
        float4 Color : COLOR;                                           \n\
    };                                                                  \n\
                                                                        \n\
    struct VertexOut                                                    \n\
    {                                                                   \n\
        float4 PosH : SV_POSITION;                                      \n\
        float4 Color : COLOR;                                           \n\
    };                                                                  \n\
                                                                        \n\
    VertexOut VS(VertexIn vin)                                          \n\
    {                                                                   \n\
        VertexOut vout;                                                 \n\
        vout.PosH = float4(vin.PosL, 1.0f);                             \n\
        vout.Color = vin.Color;                                         \n\
        return vout;                                                    \n\
    }                                                                   \n\
                                                                        \n\
    float4 PS(VertexOut pin) : SV_Target                                \n\
    {                                                                   \n\
        return pin.Color;                                               \n\
    }";

  //-------------------------------------------------------------------------
  rex::renderer::commands::CompileShaderCommandDesc create_compile_shader_parameters(const rsl::small_stack_string& shaderName, rex::renderer::ShaderType shaderType)
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

    compile_shader_command_desc.shader_code.allocate((rsl::memory_size)g_color_shader.size());
    compile_shader_command_desc.shader_code.zero_initialize();

    rex::memory::Blob::copy(g_color_shader.data(), (rsl::memory_size)g_color_shader.size(), compile_shader_command_desc.shader_code);

    compile_shader_command_desc.shader_name = shaderName;
    compile_shader_command_desc.shader_type = shaderType;

    return compile_shader_command_desc;
  }

  //-------------------------------------------------------------------------
  bool build_clear_state()
  {
    rex::renderer::commands::CreateClearStateCommandDesc create_clear_state_command_desc {};

    create_clear_state_command_desc.rgba    = rsl::colors::Red;
    create_clear_state_command_desc.depth   = 1.0f;
    create_clear_state_command_desc.stencil = 0x00;

    rex::StateController<rex::renderer::ClearBits> clear_flags;
    clear_flags.add_state(rex::renderer::ClearBits::ClearColorBuffer);
    clear_flags.add_state(rex::renderer::ClearBits::ClearDepthBuffer);
    clear_flags.add_state(rex::renderer::ClearBits::ClearStencilBuffer);

    create_clear_state_command_desc.flags = clear_flags;

    g_test_ctx.clear_state = rex::renderer::create_clear_state(rsl::move(create_clear_state_command_desc));

    return true;
  }

  //-------------------------------------------------------------------------
  bool build_shader_and_input_layout()
  {
    // Shader
    rex::renderer::commands::CompileShaderCommandDesc vs_compile_command_desc = create_compile_shader_parameters("standardVS"_small, rex::renderer::ShaderType::VERTEX);
    rex::renderer::commands::CompileShaderCommandDesc ps_compile_command_desc = create_compile_shader_parameters("opaquePS"_small, rex::renderer::ShaderType::PIXEL);

    rex::renderer::commands::LinkShaderCommandDesc link_shader_command_desc;
    link_shader_command_desc.vertex_shader = rex::renderer::compile_shader(rsl::move(vs_compile_command_desc));
    link_shader_command_desc.pixel_shader  = rex::renderer::compile_shader(rsl::move(ps_compile_command_desc));
    g_test_ctx.shader_program              = rex::renderer::link_shader(rsl::move(link_shader_command_desc));

    // Input Layout
    rex::renderer::commands::CreateInputLayoutCommandDesc input_layout_command_desc;
    input_layout_command_desc.input_layout = {rex::renderer::commands::InputLayoutDescription {"POSITION", 0, rex::renderer::VertexBufferFormat::FLOAT3, 0, 0, rex::renderer::InputLayoutClassification::PerVertexData, 0},
                                              rex::renderer::commands::InputLayoutDescription {"COLOR", 0, rex::renderer::VertexBufferFormat::FLOAT4, 0, 12, rex::renderer::InputLayoutClassification::PerVertexData, 0}};
    g_test_ctx.input_layout                = rex::renderer::create_input_layout(rsl::move(input_layout_command_desc));

    return true;
  }

    //-------------------------------------------------------------------------
  bool build_frame_resources()
  {
    for(int i = 0; i < rex::renderer::max_frames_in_flight(); ++i)
    {
      rex::renderer::ResourceSlot frame_slot = rex::renderer::create_frame_resource();

      g_test_ctx.frames.emplace_back(frame_slot);
    }

    return true;
  }

  //-------------------------------------------------------------------------
  bool build_raster_state()
  {
    rex::renderer::commands::CreateRasterStateCommandDesc solid_rs_command_desc;
    solid_rs_command_desc.fill_mode = rex::renderer::FillMode::SOLID;
    solid_rs_command_desc.cull_mode = rex::renderer::CullMode::BACK;
    g_test_ctx.solid_raster_state = rex::renderer::create_raster_state(rsl::move(solid_rs_command_desc));

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
    create_pso_command_desc.input_layout       = g_test_ctx.input_layout;
    create_pso_command_desc.num_render_targets = 1;
    create_pso_command_desc.shader_program     = g_test_ctx.shader_program;
#if RENDER_WIREFRAME
    create_pso_command_desc.rasterizer_state = g_regina_ctx.wire_raster_state;
#else
    create_pso_command_desc.rasterizer_state = g_test_ctx.solid_raster_state;
#endif
    g_test_ctx.pso = rex::renderer::create_pipeline_state_object(rsl::move(create_pso_command_desc));

    return true;
  }

  //-------------------------------------------------------------------------
  bool initialize()
  {
    if(!build_clear_state())
      return false;
    if(!build_shader_and_input_layout())
      return false;
    if(!build_frame_resources())
      return false;
    if(!build_raster_state())
      return false;
    if(!build_pipeline_state_object())
      return false;

    return true;
  }
  
  //-------------------------------------------------------------------------
  void update()
  {
    ++g_test_ctx.frame_counter;
    
    if(g_test_ctx.frame_counter > rex::renderer::max_frames_in_flight())
    {
      rex::event_system::enqueue_event(rex::event_system::EventType::QuitApp);
    }
  }

  //-------------------------------------------------------------------------
  void draw()
  {
    const rex::Viewport vp    = {0.0f, 0.0f, static_cast<f32>(rex::globals::window_info().width), static_cast<f32>(rex::globals::window_info().height), 0.0f, 1.0f};
    const rex::ScissorRect sr = {vp.top_left_x, vp.top_left_y, vp.width, vp.height};

    rex::renderer::set_pipeline_state_object(g_test_ctx.pso);
    rex::renderer::set_raster_state(g_test_ctx.solid_raster_state);

    rex::renderer::new_frame();

    rex::renderer::set_render_targets(rex::globals::default_targets_info().back_buffer_color, rex::globals::default_targets_info().depth_buffer);
    rex::renderer::set_viewport(vp);
    rex::renderer::set_scissor_rect(sr);

    rex::renderer::begin_draw();

    rex::renderer::clear(g_test_ctx.clear_state);

    rex::renderer::end_draw();

    rex::renderer::present();
    rex::renderer::end_frame();
  }
  
  //-------------------------------------------------------------------------
  void shutdown()
  {
    g_test_ctx.clear_state.release();

    g_test_ctx.shader_program.release();
    g_test_ctx.input_layout.release();
    g_test_ctx.pso.release();

    g_test_ctx.solid_raster_state.release();
  }

  //-------------------------------------------------------------------------
  rex::ApplicationCreationParams create_app_creation_params(rex::PlatformCreationParams&& platformParams)
  {
    rex::ApplicationCreationParams app_params(&platformParams);

    app_params.gui_params.window_width  = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title  = "Renderer Direct X 12 Test";

    app_params.create_window            = true;

    return app_params;
  }

  //-------------------------------------------------------------------------
  rex::ApplicationCreationParams boot_test_entry(rex::PlatformCreationParams&& platformParams)
  {
    rex::ApplicationCreationParams app_params = create_app_creation_params(rsl::move(platformParams));

    app_params.gui_params.window_title = "Renderer DirectX 12 Test";

    app_params.engine_params.app_init_func = rex_renderer_directx_build_clear_state_test::initialize;
    app_params.engine_params.app_update_func = rex_renderer_directx_build_clear_state_test::update;
    app_params.engine_params.app_draw_func = rex_renderer_directx_build_clear_state_test::draw;
    app_params.engine_params.app_shutdown_func = rex_renderer_directx_build_clear_state_test::shutdown;

    return app_params;
  }
} // namespace regina_auto_test
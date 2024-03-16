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
  class Regina
  {
  public:
    Regina()
    {}

    void init()
    {
      if (rex::cmdline::get_argument("UseCubeScene"))
      {
        m_scene = rsl::make_unique<regina::CubeScene>();
      }
      else
      {
        m_scene = rsl::make_unique<regina::SampleScene>();
      }
    }

    void update()
    {

    }

    void draw()
    {
      m_scene_renderer.render(m_scene.get(), rex::globals::window_info().width, rex::globals::window_info().height);
    }

    void shutdown()
    {

    }

  private:
    rsl::unique_ptr<rex::renderer::Scene> m_scene;
    rex::renderer::SceneRenderer m_scene_renderer;
  };

  Regina g_regina;

  //-------------------------------------------------------------------------
  bool initialize()
  {
    REX_LOG(LogRegina, "Initializing Regina");

    g_regina.init();

    return true;
  }
  //-------------------------------------------------------------------------
  void update()
  {
    g_regina.update();

    // Has the GPU finished processing the commands of the current frame resource?
    // If not, wait until the GPU has completed commands up to this fence point.
    //
    // This call is not queued but directly executed!
    rex::renderer::wait_for_active_frame();
  }
  //-------------------------------------------------------------------------
  void draw()
  {
    const f32 width = static_cast<f32>(rex::globals::window_info().width);
    const f32 height = static_cast<f32>(rex::globals::window_info().height);
    const rex::Viewport vp    = {0.0f, 0.0f, width, height, 0.0f, 1.0f};
    const rex::ScissorRect sr = {vp.top_left_x, vp.top_left_y, vp.width, vp.height};

    rex::renderer::set_viewport(vp);
    rex::renderer::set_scissor_rect(sr);

    g_regina.draw();
  }
  //-------------------------------------------------------------------------
  void shutdown()
  {
    REX_LOG(LogRegina, "shutting down Regina");

    g_regina.shutdown();
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
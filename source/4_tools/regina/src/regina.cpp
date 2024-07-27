#include "rex_engine/app/core_application.h"
#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/engine/entrypoint.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_renderer_core/gfx/depth_info.h"
#include "rex_renderer_core/rendering/renderer.h"
#include "rex_renderer_core/gfx/vertex.h"
#include "rex_renderer_core/gfx/scissor_rect.h"
#include "rex_renderer_core/gfx/viewport.h"
#include "rex_std/bonus/math/color.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/string.h"
#include "rex_windows/app/gui_application.h"
#include "rex_windows/engine/platform_creation_params.h"

#include "rex_renderer_core/rendering/scene_renderer.h"
#include "rex_renderer_core/rendering/camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "regina/sample_scene.h"
#include "regina/cube_scene.h"

DEFINE_LOG_CATEGORY(LogRegina);

namespace regina
{
  class Regina
  {
  public:
    Regina()
      : m_scene_camera(rsl::DegAngle(45.0f), 1280.0f, 720.0f, 0.1f, 1000.0f)
    {
      m_scene = rsl::make_unique<regina::CubeScene>();
      m_scene_renderer = rsl::make_unique<rex::gfx::SceneRenderer>();
      m_scene_renderer->set_scene(m_scene.get());
      m_scene_renderer->set_camera(&m_scene_camera);

      rex::gfx::add_renderer(rsl::move(m_scene_renderer));
    }

    void update()
    {
      // Based on the input, move the camera

      //draw();
    }

    void draw()
    {
      //m_scene->render(*m_scene_renderer, m_scene_camera);
      //m_editor_renderer.render();
    }

  private:
    rsl::unique_ptr<rex::gfx::Scene> m_scene;
    rsl::unique_ptr<rex::gfx::SceneRenderer> m_scene_renderer;
    rex::gfx::Camera m_scene_camera;
  };

  rsl::unique_ptr<Regina> g_regina;

  //-------------------------------------------------------------------------
  bool initialize()
  {
    REX_INFO(LogRegina, "Initializing Regina");

    // Create the editor object which will initialize it.
    g_regina = rsl::make_unique<Regina>();

    return true;
  }
  //-------------------------------------------------------------------------
  void update()
  {
    g_regina->update();
  }
  //-------------------------------------------------------------------------
  void draw()
  {
    g_regina->draw();
  }
  //-------------------------------------------------------------------------
  void shutdown()
  {
    REX_INFO(LogRegina, "shutting down Regina");

    // Shut down the editor and all its resources
    g_regina.reset();
  }

  //-------------------------------------------------------------------------
  rex::ApplicationCreationParams create_regina_app_creation_params(rex::PlatformCreationParams& platformParams)
  {
    rex::ApplicationCreationParams app_params(platformParams);

    app_params.gui_params.window_width  = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title  = rex::project_name();

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
  //rsl::string_view project_name()
  //{
  //  static rsl::string_view project_name = rex::cmdline::get_argument("project").value_or("editor_project");
  //  return project_name;
  //}

#ifndef REX_ENABLE_AUTO_TESTS
  //-------------------------------------------------------------------------
  ApplicationCreationParams app_entry(PlatformCreationParams& platformParams)
  {
    return regina::create_regina_app_creation_params(platformParams);
  }
#endif
} // namespace rex
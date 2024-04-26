#include "rex_engine/app/core_application.h"
#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/engine/entrypoint.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_renderer_core/rendering/depth_info.h"
#include "rex_renderer_core/system/renderer.h"
#include "rex_renderer_core/rendering/scene.h"
#include "rex_renderer_core/resources/mesh.h"
#include "rex_renderer_core/rendering/vertex.h"
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

DEFINE_LOG_CATEGORY(LogRegina);

namespace regina
{
  class Regina
  {
  public:
    Regina()
    {
      m_scene = rsl::make_unique<regina::CubeScene>();
    }

    void update()
    {
      // Nothing to implement
    }

    void draw()
    {
      m_scene->update();
    }

  private:
    rsl::unique_ptr<rex::renderer::Scene> m_scene;
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
    // Update the editor
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
  ApplicationCreationParams app_entry(PlatformCreationParams& platformParams)
  {
    return regina::create_regina_app_creation_params(platformParams);
  }
#endif
} // namespace rex
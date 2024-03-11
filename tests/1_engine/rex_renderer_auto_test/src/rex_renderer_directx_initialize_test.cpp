#include "rex_renderer_auto_test/rex_renderer_directx_initialize_test.h"
#include "rex_engine/event_system/event_system.h"
#include "rex_windows/platform_creation_params.h"
#include "rex_renderer_core/renderer.h"

namespace rex_renderer_directx_initialize_test
{
  struct TestContext
  {
    s32 frame_counter = 0;
  };

  TestContext g_test_ctx; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

  //-------------------------------------------------------------------------
  bool initialize()
  {
    // Nothing to implement

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
  void shutdown()
  {
    // Nothing to implement
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

    app_params.engine_params.app_init_func = rex_renderer_directx_initialize_test::initialize;
    app_params.engine_params.app_update_func = rex_renderer_directx_initialize_test::update;
    app_params.engine_params.app_shutdown_func = rex_renderer_directx_initialize_test::shutdown;

    return app_params;
  }
} // namespace regina_auto_test
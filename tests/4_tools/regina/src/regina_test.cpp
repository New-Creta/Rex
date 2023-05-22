#include "regina/regina.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/entrypoint.h"
#include "rex_windows/gui_application.h"
#include "rex_engine/event_system.h"

namespace regina_test
{
  bool initialize()
  {
    return rex::initialize();
  }
  void update()
  {
    rex::event_system::Event ev{};
    ev.type = rex::event_system::EventType::WindowClose;
    rex::event_system::fire_event(ev);
  }
  void shutdown()
  {
    rex::shutdown();
  }

  rex::ApplicationCreationParams app_entry(rex::PlatformCreationParams&& platformParams, rex::CommandLineArguments&& cmdArgs)
  {
    rex::ApplicationCreationParams app_params(rsl::move(platformParams), rsl::move(cmdArgs));

    app_params.gui_params.window_width  = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title  = "Regina Test"_sid;

    app_params.engine_params.max_memory        = 256_kb;
    app_params.engine_params.app_init_func     = initialize;
    app_params.engine_params.app_update_func   = update;
    app_params.engine_params.app_shutdown_func = shutdown;

    return app_params;
  }
}
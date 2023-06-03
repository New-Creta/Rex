#include "regina/regina.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/entrypoint.h"
#include "rex_engine/event_system.h"
#include "rex_windows/gui_application.h"

namespace regina_test
{
  bool initialize()
  {
    return rex::initialize();
  }
  void update()
  {
    rex::event_system::Event ev {};
    ev.type = rex::event_system::EventType::QuitApp;
    rex::event_system::fire_event(ev);
  }
  void shutdown()
  {
    rex::shutdown();
  }
} // namespace regina_test

namespace rex
{
  rex::ApplicationCreationParams app_entry(rex::PlatformCreationParams&& platformParams, rex::CommandLineArguments&& cmdArgs)
  {
    rex::ApplicationCreationParams app_params = create_regina_app_creation_params(rsl::move(platformParams), rsl::move(cmdArgs));

    // overwrite settings for auto tests
    app_params.gui_params.window_title  = "Regina Test";

    app_params.engine_params.app_init_func     = regina_test::initialize;
    app_params.engine_params.app_update_func   = regina_test::update;
    app_params.engine_params.app_shutdown_func = regina_test::shutdown;

    return app_params;
  }
} // namespace rex
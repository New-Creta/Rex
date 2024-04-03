#include "regina_auto_test/regina_boot_test.h"
#include "regina/regina.h"
#include "rex_engine/event_system/event_system.h"
#include "rex_windows/engine/platform_creation_params.h"

namespace my_auto_tests
{
  // We simply want to initialize the editor and do nothing else
  bool initialize()
  {
    return regina::initialize();
  }
  void update()
  {
    // On the first tick, we want to send the quit event
    // Booting has finished, we can shut down
    rex::event_system::enqueue_event(rex::event_system::EventType::QuitApp);
  }
  void shutdown()
  {
    regina::shutdown();
  }

  // This acts as the entry function for the auto test.
  // We simply fill in the functions of the auto test and return
  rex::ApplicationCreationParams boot_test_entry(rex::PlatformCreationParams&& platformParams)
  {
    rex::ApplicationCreationParams app_params = regina::create_regina_app_creation_params(rsl::move(platformParams));

    app_params.gui_params.window_title = "Regina Test";

    app_params.engine_params.app_init_func = regina_auto_test::initialize;
    app_params.engine_params.app_update_func = regina_auto_test::update;
    app_params.engine_params.app_shutdown_func = regina_auto_test::shutdown;

    return app_params;
  }
} // namespace regina_auto_test
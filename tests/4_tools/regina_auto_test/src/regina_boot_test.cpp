#include "regina_auto_test/regina_boot_test.h"
#include "regina/regina.h"
#include "rex_engine/event_system.h"
#include "rex_windows/platform_creation_params.h"

namespace regina_auto_test
{
  bool initialize()
  {
    return rex::initialize();
  }
  void update()
  {
    rex::event_system::Event ev{};
    ev.type = rex::event_system::EventType::QuitApp;
    rex::event_system::fire_event(ev);
  }
  void shutdown()
  {
    rex::shutdown();
  }

  rex::ApplicationCreationParams boot_test_entry(rex::PlatformCreationParams&& platformParams)
  {
    rex::ApplicationCreationParams app_params = create_regina_app_creation_params(rsl::move(platformParams));

    app_params.gui_params.window_title = "Regina Test";

    app_params.engine_params.app_init_func = regina_auto_test::initialize;
    app_params.engine_params.app_update_func = regina_auto_test::update;
    app_params.engine_params.app_shutdown_func = regina_auto_test::shutdown;

    return app_params;
  }
} // namespace regina_auto_test
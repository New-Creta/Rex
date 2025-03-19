#include "rex_engine_auto_test/rex_engine_auto_test_gui_app.h"

#include "rex_engine/event_system/event_system.h"
#include "rex_engine/event_system/events/app/quit_app.h"

#include "rex_engine/engine/engine_params.h"

namespace rex
{
	namespace gui_app_boot_test
	{
		bool initialize(const ApplicationCreationParams&)
		{
			// Nothing to initialize here, we just want to make sure the engine systems are able to initialize a gui app
			return true;
		}
		void update()
		{
			rex::event_system().enqueue_event(rex::QuitApp("Exiting GUI app autotest"));
		}
		void shutdown()
		{
			// Nothing to implement
		}

		rex::ApplicationCreationParams gui_app_test_entry(rex::PlatformCreationParams& platformParams)
		{
			rex::ApplicationCreationParams app_params(platformParams);

			app_params.gui_params.window_title = "GUI App Test";
			app_params.is_gui_app = true;

			app_params.engine_params.app_init_func = gui_app_boot_test::initialize;
			app_params.engine_params.app_update_func = gui_app_boot_test::update;
			app_params.engine_params.app_shutdown_func = gui_app_boot_test::shutdown;

			return app_params;
		}
	}
}
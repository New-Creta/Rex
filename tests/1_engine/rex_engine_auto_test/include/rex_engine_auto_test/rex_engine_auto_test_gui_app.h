#pragma once

namespace rex
{
	struct PlatformCreationParams;
	struct ApplicationCreationParams;

	namespace gui_app_boot_test
	{
		bool initialize(const ApplicationCreationParams&);
		void update();
		void shutdown();

		rex::ApplicationCreationParams gui_app_test_entry(rex::PlatformCreationParams& platformParams);
	}
}
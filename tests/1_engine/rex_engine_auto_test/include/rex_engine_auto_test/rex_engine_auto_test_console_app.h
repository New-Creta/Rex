#pragma once

namespace rex
{
	struct PlatformCreationParams;
	struct ApplicationCreationParams;

	namespace console_app_boot_test
	{
		bool initialize(const ApplicationCreationParams&);
		void update();
		void shutdown();

		rex::ApplicationCreationParams console_app_test_entry(rex::PlatformCreationParams& platformParams);
	}
}
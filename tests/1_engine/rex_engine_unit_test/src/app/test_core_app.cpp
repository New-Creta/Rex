#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/app/core_application.h"
#include "rex_engine/engine/engine_params.h"

namespace rex
{
	struct PlatformCreationParams {};
}

namespace rex::test
{
	class TestCoreApp : public rex::CoreApplication
	{
	public:
		TestCoreApp(ApplicationCreationParams&& appCreationParams)
			: CoreApplication(appCreationParams.engine_params)
			, m_app_creation_params(rsl::move(appCreationParams))
		{
			m_init_func = m_app_creation_params.engine_params.app_init_func;
			m_update_func = m_app_creation_params.engine_params.app_update_func;
			m_shutdown_func = m_app_creation_params.engine_params.app_shutdown_func;
		}
	protected:
		bool platform_init() override { return m_init_func(m_app_creation_params); }
		void platform_update() override { m_update_func(); }
		void platform_shutdown() override { m_shutdown_func(); }

	private:
		ApplicationCreationParams m_app_creation_params;
		rex::EngineParams::init_func m_init_func;
		rex::EngineParams::update_func m_update_func;
		rex::EngineParams::shutdown_func m_shutdown_func;
	};

	rex::ApplicationCreationParams test_core_app_creation_params()
	{
		static PlatformCreationParams platform_creation_params{};
		rex::ApplicationCreationParams app_params(platform_creation_params);

		app_params.gui_params.window_title.assign("test core app");

		return app_params;
	}
}

TEST_CASE("TEST - Core Application - Construction")
{
	rex::ApplicationCreationParams app_creation_params = rex::test::test_core_app_creation_params();
	rex::test::TestCoreApp app(rsl::move(app_creation_params));

	REX_CHECK(app.is_paused() == false);
	REX_CHECK(app.is_running() == false);
	REX_CHECK(app.is_marked_for_destroy() == false);
}

TEST_CASE("TEST - Core Application - Failed Initialization")
{
	rex::ApplicationCreationParams app_creation_params = rex::test::test_core_app_creation_params();

	app_creation_params.engine_params.app_init_func = [](const rex::ApplicationCreationParams&) { return false; };
	app_creation_params.engine_params.app_update_func = []() {};
	app_creation_params.engine_params.app_shutdown_func = []() {};

	rex::test::TestCoreApp app(rsl::move(app_creation_params));

	s32 return_code = app.run();

	REX_CHECK(return_code == EXIT_FAILURE);
	REX_CHECK(app.is_paused() == false);
	REX_CHECK(app.is_running() == false);
	REX_CHECK(app.is_marked_for_destroy() == false);
}

TEST_CASE("TEST - Core Application - Successful Initialization")
{
	rex::ApplicationCreationParams app_creation_params = rex::test::test_core_app_creation_params();
	rex::test::TestCoreApp* app_ptr = nullptr;

	app_creation_params.engine_params.app_init_func = [](const rex::ApplicationCreationParams&) { return true; };
	app_creation_params.engine_params.app_update_func = [&app_ptr]() { app_ptr->quit(); };
	app_creation_params.engine_params.app_shutdown_func = []() {};

	rex::test::TestCoreApp app(rsl::move(app_creation_params));
	app_ptr = &app;

	s32 return_code = app.run();

	REX_CHECK(return_code == EXIT_SUCCESS);
	REX_CHECK(app.is_paused() == false);
	REX_CHECK(app.is_running() == false);
	REX_CHECK(app.is_marked_for_destroy() == false);
}

TEST_CASE("TEST - Core Application - Failed Run")
{
	rex::ApplicationCreationParams app_creation_params = rex::test::test_core_app_creation_params();
	rex::test::TestCoreApp* app_ptr = nullptr;

	s32 error_code = 1;

	app_creation_params.engine_params.app_init_func = [](const rex::ApplicationCreationParams&) { return true; };
	app_creation_params.engine_params.app_update_func = [&app_ptr, error_code]() { app_ptr->quit("Exiting unit test", error_code); };
	app_creation_params.engine_params.app_shutdown_func = []() {};

	rex::test::TestCoreApp app(rsl::move(app_creation_params));
	app_ptr = &app;

	s32 return_code = app.run();

	REX_CHECK(return_code == error_code);
	REX_CHECK(app.is_paused() == false);
	REX_CHECK(app.is_running() == false);
	REX_CHECK(app.is_marked_for_destroy() == false);

}

TEST_CASE("TEST - Core Application - Successful Run")
{
	rex::ApplicationCreationParams app_creation_params = rex::test::test_core_app_creation_params();
	rex::test::TestCoreApp* app_ptr = nullptr;

	app_creation_params.engine_params.app_init_func = [](const rex::ApplicationCreationParams&) { return true; };
	app_creation_params.engine_params.app_update_func = [&app_ptr]() { app_ptr->quit(); };
	app_creation_params.engine_params.app_shutdown_func = []() {};

	rex::test::TestCoreApp app(rsl::move(app_creation_params));
	app_ptr = &app;

	s32 return_code = app.run();

	REX_CHECK(return_code == EXIT_SUCCESS);
	REX_CHECK(app.is_paused() == false);
	REX_CHECK(app.is_running() == false);
	REX_CHECK(app.is_marked_for_destroy() == false);
}

TEST_CASE("TEST - Core Application - State Checking")
{
	rex::test::TestCoreApp* app_ptr = nullptr;
	rex::ApplicationCreationParams app_creation_params = rex::test::test_core_app_creation_params();

	app_creation_params.engine_params.app_init_func = [&app_ptr](const rex::ApplicationCreationParams&)
	{
		REX_CHECK(app_ptr->is_initializing() == true);
		REX_CHECK(app_ptr->is_paused() == false);
		REX_CHECK(app_ptr->is_running() == false);
		REX_CHECK(app_ptr->is_marked_for_destroy() == false);
		REX_CHECK(app_ptr->is_shutting_down() == false);

		return true;
	};
	
	app_creation_params.engine_params.app_update_func = [&app_ptr]()
	{
		REX_CHECK(app_ptr->is_initializing() == false);
		REX_CHECK(app_ptr->is_paused() == false);
		REX_CHECK(app_ptr->is_running() == true);
		REX_CHECK(app_ptr->is_marked_for_destroy() == false);
		REX_CHECK(app_ptr->is_shutting_down() == false);

		app_ptr->pause();

		REX_CHECK(app_ptr->is_initializing() == false);
		REX_CHECK(app_ptr->is_paused() == true);
		REX_CHECK(app_ptr->is_running() == true);
		REX_CHECK(app_ptr->is_marked_for_destroy() == false);
		REX_CHECK(app_ptr->is_shutting_down() == false);

		app_ptr->quit();

		REX_CHECK(app_ptr->is_initializing() == false);
		REX_CHECK(app_ptr->is_paused() == false);
		REX_CHECK(app_ptr->is_running() == false);
		REX_CHECK(app_ptr->is_marked_for_destroy() == true);
		REX_CHECK(app_ptr->is_shutting_down() == false);

	};
	app_creation_params.engine_params.app_shutdown_func = [&app_ptr]()
	{
		REX_CHECK(app_ptr->is_initializing() == false);
		REX_CHECK(app_ptr->is_paused() == false);
		REX_CHECK(app_ptr->is_running() == false);
		REX_CHECK(app_ptr->is_marked_for_destroy() == false);
		REX_CHECK(app_ptr->is_shutting_down() == true);
	};

	auto start = rsl::chrono::high_resolution_clock::now();

	rex::test::TestCoreApp app(rsl::move(app_creation_params));
	app_ptr = &app;
	s32 return_code = app.run();

	auto end = rsl::chrono::high_resolution_clock::now();

	REX_CHECK(return_code == EXIT_SUCCESS);
	REX_CHECK(app.is_initializing() == false);
	REX_CHECK(app.is_paused() == false);
	REX_CHECK(app.is_running() == false);
	REX_CHECK(app.is_marked_for_destroy() == false);
	REX_CHECK(app.is_shutting_down() == true);
}
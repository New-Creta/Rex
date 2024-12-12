#include "rex_unit_test/rex_catch2.h"

#include "rex_engine/app/core_application.h"
#include "rex_engine/engine/engine_params.h"

namespace rex::test
{
	class TestCoreApp : public rex::CoreApplication
	{
	public:
		TestCoreApp(const rex::EngineParams& params)
			: CoreApplication(params)
		{}

	protected:
		bool platform_init() override { return true; }
		void platform_update() override {}
		void platform_shutdown() override {}

	private:
	};
}

TEST_CASE("Core Application Construction")
{
	rex::EngineParams engine_params;
	rex::test::TestCoreApp app(engine_params);

	REX_CHECK(app.is_paused() == false);
	REX_CHECK(app.is_running() == false);
	REX_CHECK(app.is_marked_for_destroy() == false);
}

TEST_CASE("Core Application Failed Initialization")
{
	rex::EngineParams engine_params;
	engine_params.app_init_func = [](const rex::ApplicationCreationParams&) { return false; };
	engine_params.app_update_func = []() {};
	engine_params.app_shutdown_func = []() {};

	rex::test::TestCoreApp app(engine_params);

	s32 return_code = app.run();

	REX_CHECK(return_code == EXIT_FAILURE);
	REX_CHECK(app.is_paused() == false);
	REX_CHECK(app.is_running() == false);
	REX_CHECK(app.is_marked_for_destroy() == false);
}

TEST_CASE("Core Application Successful Initialization")
{
	rex::EngineParams engine_params;
	engine_params.app_init_func = [](const rex::ApplicationCreationParams&) { return true; };
	engine_params.app_update_func = []() {};
	engine_params.app_shutdown_func = []() {};

	rex::test::TestCoreApp app(engine_params);

	s32 return_code = app.run();

	REX_CHECK(return_code == EXIT_SUCCESS);
	REX_CHECK(app.is_paused() == false);
	REX_CHECK(app.is_running() == false);
	REX_CHECK(app.is_marked_for_destroy() == false);
}

TEST_CASE("Core Application State Checking")
{
	rex::test::TestCoreApp* app_ptr = nullptr;

	rex::EngineParams engine_params;
	engine_params.app_init_func = [app_ptr](const rex::ApplicationCreationParams&) 
	{
		REX_CHECK(app_ptr->is_initializing() == true);
		REX_CHECK(app_ptr->is_paused() == false);
		REX_CHECK(app_ptr->is_running() == true);
		REX_CHECK(app_ptr->is_marked_for_destroy() == false);
		REX_CHECK(app_ptr->is_shutting_down() == false);

		return true;
	};
	
	engine_params.app_update_func = [app_ptr]() 
	{
		REX_CHECK(app_ptr->is_initializing() == false);
		REX_CHECK(app_ptr->is_paused() == false);
		REX_CHECK(app_ptr->is_running() == true);
		REX_CHECK(app_ptr->is_marked_for_destroy() == false);
		REX_CHECK(app_ptr->is_shutting_down() == false);

		app_ptr->pause();

		REX_CHECK(app_ptr->is_initializing() == false);
		REX_CHECK(app_ptr->is_paused() == true);
		REX_CHECK(app_ptr->is_running() == false);
		REX_CHECK(app_ptr->is_marked_for_destroy() == false);
		REX_CHECK(app_ptr->is_shutting_down() == false);

		app_ptr->quit();

		REX_CHECK(app_ptr->is_initializing() == false);
		REX_CHECK(app_ptr->is_paused() == false);
		REX_CHECK(app_ptr->is_running() == false);
		REX_CHECK(app_ptr->is_marked_for_destroy() == true);
		REX_CHECK(app_ptr->is_shutting_down() == false);

	};
	engine_params.app_shutdown_func = [app_ptr]() 
	{
		REX_CHECK(app_ptr->is_initializing() == false);
		REX_CHECK(app_ptr->is_paused() == false);
		REX_CHECK(app_ptr->is_running() == false);
		REX_CHECK(app_ptr->is_marked_for_destroy() == false);
		REX_CHECK(app_ptr->is_shutting_down() == true);
	};

	rex::test::TestCoreApp app(engine_params);
	app_ptr = &app;
	s32 return_code = app.run();

	REX_CHECK(return_code == EXIT_SUCCESS);
	REX_CHECK(app.is_initializing() == false);
	REX_CHECK(app.is_paused() == false);
	REX_CHECK(app.is_running() == false);
	REX_CHECK(app.is_marked_for_destroy() == false);
	REX_CHECK(app.is_shutting_down() == true);
}
#include "regina/regina_entry.h"
#include "regina/regina.h"
#include "regina/project_manager.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/log.h"

namespace regina
{
  DEFINE_LOG_CATEGORY(LogReginaEntry);

  rsl::unique_ptr<Regina> g_regina;

  //-------------------------------------------------------------------------
  bool create_editor(const rex::ApplicationCreationParams& /*appCreationParams*/)
  {
    REX_INFO(LogReginaEntry, "Initializing Regina");

    // Load the project from disk if there's one specified
    rsl::unique_ptr<Project> project;
    if (rex::cmdline::get_argument("project").has_value())
    {
      project = project_manager::load_from_disk(rex::cmdline::get_argument("project").value());
      if (project == nullptr)
      {
        // Failed to load the project -> fail initialization
        REX_ERROR(LogReginaEntry, "Failed to load project from disk");
        return false;
      }
      REX_INFO(LogReginaEntry, "Project \"{}\" loaded", project->name());
    }

    g_regina = rsl::make_unique<Regina>(rsl::move(project));

    REX_INFO(LogReginaEntry, "Regina initialized");

    return true;
  }
  //-------------------------------------------------------------------------
  void update_editor()
  {
    g_regina->update();
  }
  //-------------------------------------------------------------------------
  void destroy_editor()
  {
    REX_INFO(LogReginaEntry, "shutting down Regina");

    // Shut down the editor and all its resources
    g_regina.reset();
  }

  //-------------------------------------------------------------------------
// This is basically the entry point into the app after the engine has done the first setup
  rex::ApplicationCreationParams create_regina_app_creation_params(rex::PlatformCreationParams& platformParams)
  {
    rex::ApplicationCreationParams app_params(platformParams);

    app_params.gui_params.window_width = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title.assign(rsl::format("Regina: {}", rex::project_name()));

    app_params.engine_params.app_init_func = create_editor;
    app_params.engine_params.app_update_func = update_editor;
    app_params.engine_params.app_shutdown_func = destroy_editor;
    app_params.engine_params.app_name.assign("Rex Editor");
    app_params.is_gui_app = true;

    return app_params;
  }
}

namespace rex
{
  bool g_project_name_set = set_project_name("Pokemon");

#ifndef REX_ENABLE_AUTO_TESTS
  //-------------------------------------------------------------------------
  ApplicationCreationParams app_entry(PlatformCreationParams& platformParams)
  {
    return regina::create_regina_app_creation_params(platformParams);
  }
#endif
} // namespace rex
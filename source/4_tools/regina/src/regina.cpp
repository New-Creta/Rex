#include "rex_engine/app/core_application.h"
#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/engine/entrypoint.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_engine/gfx/core/depth_info.h"
#include "rex_engine/gfx/rendering/renderer.h"
#include "rex_engine/gfx/core/vertex.h"
#include "rex_engine/gfx/core/scissor_rect.h"
#include "rex_engine/gfx/core/viewport.h"
#include "rex_std/bonus/math/color.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/string.h"
#include "rex_windows/app/gui_application.h"
#include "rex_windows/engine/platform_creation_params.h"

#include "rex_engine/memory/memory_tracking.h"

#include "rex_engine/event_system/event_system.h"
#include "rex_engine/event_system/events/app/quit_app.h"

#include "rex_engine/gfx/rendering/scene_renderer.h"
#include "rex_engine/gfx/rendering/camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "regina/sample_scene2.h"
#include "regina/sample_scene.h"
#include "regina/cube_scene.h"
#include "regina/project.h"
#include "regina/content_manager.h"

DEFINE_LOG_CATEGORY(LogRegina);

#include "imgui/imgui.h"

namespace regina
{
  class Regina
  {
  public:
    Regina(rsl::unique_ptr<Project> project, s32 windowWidth, s32 windowHeight)
      : m_project(rsl::move(project))
    {
      if (!project)
      {
        create_new_project();
      }
    }

    void update()
    {
      show_menu_bar();
      show_content_browser();

      ImGui::ShowDemoWindow();
    }

  private:
    void show_menu_bar()
    {
      if (ImGui::BeginMainMenuBar())
      {
        if (ImGui::BeginMenu("File"))
        {
          if (ImGui::MenuItem("Quit"))
          {
            rex::event_system().enqueue_event(rex::QuitApp("Quit Pressed In Menu"));
          }
          ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
      }
    }
    void show_content_browser()
    {
      m_content_manager = rsl::make_unique<ContentManager>();
      m_content_manager->add_content(rex::vfs::project_root());
    }
    void create_new_project()
    {
      spawn_create_project_widget();
    }
    void spawn_create_project_widget()
    {

    }

  private:
    rsl::unique_ptr<Project> m_project;
    rsl::unique_ptr<ContentManager> m_content_manager;
  };

  rsl::unique_ptr<Regina> g_regina;

  rsl::unique_ptr<Project> load_project_from_disk(rsl::string_view projectName)
  {
    if (projectName.empty())
    {
      REX_ERROR(LogRegina, "No project name specfied");
      return nullptr;
    }

    // don't add the extension if it's already added on the commandline
    rsl::string project_filename(projectName);
    rsl::string_view project_extension = ".rexproj";
    if (!projectName.ends_with(project_extension))
    {
      project_filename += project_extension;
    }

    // verify that the project path actually exist
    rsl::string project_filepath = rex::vfs::abs_path(rex::path::join(projectName, project_filename));
    if (!rex::vfs::is_file(project_filepath))
    {
      REX_ERROR(LogRegina, "Project \"{}\" does not exist", project_filepath);
      return nullptr;
    }

    return rsl::make_unique<Project>(project_filepath);
  }

  //-------------------------------------------------------------------------
  bool create_editor(const rex::ApplicationCreationParams& appCreationParams)
  {
    REX_INFO(LogRegina, "Initializing Regina");

    // Load the project from disk if there's one specified
    rsl::unique_ptr<Project> project;
    if (rex::cmdline::get_argument("project").has_value())
    {
      project = load_project_from_disk(rex::cmdline::get_argument("project").value());
      if (project == nullptr)
      {
        // Failed to load the project -> fail initialization
        return false;
      }
      REX_INFO(LogRegina, "Project {} loaded", project->name());
    }

    s32 window_width = appCreationParams.gui_params.window_width;
    s32 window_height = appCreationParams.gui_params.window_height;

    g_regina = rsl::make_unique<Regina>(rsl::move(project), window_width, window_height);

    REX_INFO(LogRegina, "Regina initialized");

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
    REX_INFO(LogRegina, "shutting down Regina");

    // Shut down the editor and all its resources
    g_regina.reset();
  }

  //-------------------------------------------------------------------------
  rex::ApplicationCreationParams create_regina_app_creation_params(rex::PlatformCreationParams& platformParams)
  {
    rex::ApplicationCreationParams app_params(platformParams);

    app_params.gui_params.window_width  = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title.assign(rsl::format("Regina: {}", rex::project_name()));

    app_params.engine_params.app_init_func     = create_editor;
    app_params.engine_params.app_update_func   = update_editor;
    app_params.engine_params.app_shutdown_func = destroy_editor;
    app_params.engine_params.app_name.assign("Rex Editor");
    app_params.is_gui_app                   = true;

    return app_params;
  }

} // namespace regina

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
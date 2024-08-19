#include "rex_engine/app/core_application.h"
#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/engine/entrypoint.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_renderer_core/gfx/depth_info.h"
#include "rex_renderer_core/rendering/renderer.h"
#include "rex_renderer_core/gfx/vertex.h"
#include "rex_renderer_core/gfx/scissor_rect.h"
#include "rex_renderer_core/gfx/viewport.h"
#include "rex_std/bonus/math/color.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/string.h"
#include "rex_windows/app/gui_application.h"
#include "rex_windows/engine/platform_creation_params.h"

#include "rex_engine/memory/memory_tracking.h"

#include "rex_renderer_core/rendering/scene_renderer.h"
#include "rex_renderer_core/rendering/camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "regina/sample_scene2.h"
#include "regina/sample_scene.h"
#include "regina/cube_scene.h"

DEFINE_LOG_CATEGORY(LogRegina);

#include "imgui/imgui.h"

namespace regina
{
  class Regina
  {
  public:
    Regina(s32 windowWidth, s32 windowHeight)
      : m_scene_camera(glm::vec3(0.0f, 5.0f, -20.0f), rsl::DegAngle(45.0f), static_cast<f32>(windowWidth), static_cast<f32>(windowHeight), 0.1f, 1000.0f)
      , m_scene_viewport_width(windowWidth)
      , m_scene_viewport_height(windowHeight)
    {
      m_scene = rsl::make_unique<regina::SampleScene>();
      m_scene_renderer = rex::gfx::add_renderer<rex::gfx::SceneRenderer>();
      m_light_direction.y = -1.0f;
      m_light_direction.z = 2.0f;
    }

    void update()
    {
      update_scene_renderer();

      ImGui::ShowDemoWindow();

      static bool orto_cam = false;
      ImGui::Checkbox("Orthographic Camera", &orto_cam);
      if (orto_cam)
      {
        m_scene_camera.switch_mode(rex::gfx::ProjectionMode::Ortographic);
      }
      else
      {
        m_scene_camera.switch_mode(rex::gfx::ProjectionMode::Perspective);
      }

      if (ImGui::Button("Dump mem states"))
      {
        rex::mem_tracker().dump_stats_to_file("mem_stats.txt");
      }

      ImGui::DragFloat3("Light Direction", &m_light_direction.r, 0.01f);
    }

  private:
    void update_scene_renderer()
    {
      rex::gfx::SceneData scene_data{};
      scene_data.scene = m_scene.get();
      scene_data.camera = &m_scene_camera;
      scene_data.viewport_width = m_scene_viewport_width;
      scene_data.viewport_height = m_scene_viewport_height;
      scene_data.light_direction = m_light_direction;
      m_scene_renderer->update_scene_data(scene_data);
    }

  private:
    rsl::unique_ptr<rex::gfx::Scene> m_scene;
    rex::gfx::SceneRenderer* m_scene_renderer;
    rex::gfx::Camera m_scene_camera;
    s32 m_scene_viewport_width;
    s32 m_scene_viewport_height;
    glm::vec3 m_light_direction;
  };

  rsl::unique_ptr<Regina> g_regina;

  //-------------------------------------------------------------------------
  bool create_editor(const rex::ApplicationCreationParams& appCreationParams)
  {
    REX_INFO(LogRegina, "Initializing Regina");

    // Create the editor object which will create_editor it.
    s32 window_width = appCreationParams.gui_params.window_width;
    s32 window_height = appCreationParams.gui_params.window_height;
    g_regina = rsl::make_unique<Regina>(window_width, window_height);

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
    app_params.gui_params.window_title  = rex::project_name();

    app_params.engine_params.app_init_func     = create_editor;
    app_params.engine_params.app_update_func   = update_editor;
    app_params.engine_params.app_shutdown_func = destroy_editor;
    app_params.create_window                   = true;

    return app_params;
  }

} // namespace regina

namespace rex
{
  //rsl::string_view project_name()
  //{
  //  static rsl::string_view project_name = rex::cmdline::get_argument("project").value_or("editor_project");
  //  return project_name;
  //}

#ifndef REX_ENABLE_AUTO_TESTS
  //-------------------------------------------------------------------------
  ApplicationCreationParams app_entry(PlatformCreationParams& platformParams)
  {
    return regina::create_regina_app_creation_params(platformParams);
  }
#endif
} // namespace rex
#include "imgui.h"
#include "rex_directx/scenegraph/camera_node.h"
#include "rex_directx/scenegraph/mesh_node.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/engine/entrypoint.h"
#include "rex_engine/scenegraph/scene_manager.h"

#include "rex_directx/scenegraph/camera_node.h"
#include "rex_directx/scenegraph/mesh_node.h"
#include "rex_renderer_core/viewport.h"
#include "rex_renderer_core/scissor_rect.h"
#include "rex_renderer_core/mesh_factory.h"
#include "rex_renderer_core/viewport.h"
#include "rex_windows/engine/platform_creation_params.h"
#include "rex_engine/windowinfo.h"
#include "rex_windows/app/gui_application.h"

#include <glm/gtc/matrix_transform.hpp>

DEFINE_LOG_CATEGORY(LogTechDemo, rex::LogVerbosity::Log);

namespace tech_demo
{
  struct Geometry
  {
    rsl::shared_ptr<rex::SceneNode> cube;
    rsl::shared_ptr<rex::SceneNode> sphere;
    rsl::shared_ptr<rex::SceneNode> cylinder;
    rsl::shared_ptr<rex::SceneNode> plane;
  };

  rsl::unique_ptr<Geometry> g_geometry;

  //-------------------------------------------------------------------------
  bool initialize()
  {
    REX_LOG(LogTechDemo, "Initializing Tech Demo");

    g_geometry = rsl::make_unique<Geometry>();

    g_geometry->cube     = rex::mesh_factory::create_cube(glm::vec3(0.85f, 0.0f, 0.0f));
    g_geometry->sphere   = rex::mesh_factory::create_sphere(glm::vec3(0.0f, 0.85f, 0.0f));
    g_geometry->cylinder = rex::mesh_factory::create_cylinder(glm::vec3(0.0f, 0.0f, 0.85f));
    g_geometry->plane    = rex::mesh_factory::create_plane(glm::vec3(0.85f, 0.85f, 0.85f));

    rex::SceneNode* root_node = rex::SceneManager::instance()->active_scene()->root_node();

    auto camera = rsl::make_shared<rex::CameraNode>();

    glm::vec3 camera_pos    = glm::vec3(0.0f, 5.0f, -25.0f);
    glm::vec3 camera_target = glm::vec3(0.0f, 5.0f, 0.0f);
    glm::vec3 camera_up     = glm::vec3(0.0f, 1.0f, 0.0f);

    camera->set_lookat(camera_pos, camera_target, camera_up);
    camera->set_projection(55.0f, rex::globals::window_info().width / rex::globals::window_info().height, 0.1f, 100.0f);

    root_node->add_child(camera);

    glm::mat4 mat_trans = glm::mat4(1.0f);
    glm::mat4 mat_rot   = glm::mat4(1.0f);
    glm::mat4 mat_scale = glm::mat4(1.0f);
    glm::mat4 mat_world = glm::mat4(1.0f);

    mat_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    mat_scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    mat_world = mat_trans * mat_rot * mat_scale;

    g_geometry->cube->set_local_transform(mat_world);

    root_node->add_child(g_geometry->cube);

    return true;
  }
  //-------------------------------------------------------------------------
  void update() {}
  //-------------------------------------------------------------------------
  void on_gui()
  {
    static bool show_demo_window = true;

    if(show_demo_window)
    {
      ImGui::ShowDemoWindow(&show_demo_window);
    }
  }
  //-------------------------------------------------------------------------
  void shutdown()
  {
    REX_LOG(LogTechDemo, "shutting down Tech Demo");

    g_geometry.reset();
  }

  //-------------------------------------------------------------------------
  rex::ApplicationCreationParams create_tech_demo_app_creation_params(rex::PlatformCreationParams&& platformParams)
  {
    rex::ApplicationCreationParams app_params(&platformParams);

    app_params.gui_params.window_width  = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title  = "TechDemo";

    app_params.engine_params.app_init_func   = initialize;
    app_params.engine_params.app_update_func = update;
    // app_params.engine_params.app_draw_func      = draw;
    app_params.engine_params.app_gui_func      = on_gui;
    app_params.engine_params.app_shutdown_func = shutdown;
    app_params.create_window                   = true;

    return app_params;
  }

} // namespace tech_demo

namespace rex
{
#ifndef REX_ENABLE_AUTO_TESTS
  //-------------------------------------------------------------------------
  ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams)
  {
    return tech_demo::create_tech_demo_app_creation_params(rsl::move(platformParams));
  }
#endif
} // namespace rex
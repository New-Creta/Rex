#pragma once

#include "rex_engine/engine/types.h"

#include "rex_std/memory.h"

namespace rex
{
  struct ApplicationCreationParams;
  struct PlatformCreationParams;
} // namespace rex

namespace regina
{
  class ContentManager;
  class Widget;
  class Project;

  class Regina
  {
  public:
    Regina(rsl::unique_ptr<Project> project);
    ~Regina();

    void update();

  private:
    // Initialization
    void spawn_main_widget();

    // Project management
    void create_new_project(rsl::string_view projectName);
    void spawn_create_project_widget();

  private:
    rsl::unique_ptr<Project> m_project;
    rsl::unique_ptr<ContentManager> m_content_manager;
    rsl::unique_ptr<Widget> m_active_widget;
  };

} // namespace regina
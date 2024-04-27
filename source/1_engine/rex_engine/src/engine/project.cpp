#include "rex_engine/engine/project.h"

namespace rex
{
  rsl::string g_project_name;

  bool set_project_name(rsl::string_view projectName)
  {
    g_project_name.assign(projectName);
    return true;
  }

  rsl::string_view project_name()
  {
    return g_project_name;
  }
}
#include "rex_engine/engine/project.h"

namespace rex
{
  namespace internal
  {
    rsl::string& project_name()
    {
      static rsl::string project_name;
      return project_name;
    }
  }

  bool set_project_name(rsl::string_view projectName)
  {
    internal::project_name().assign(projectName);
    return true;
  }

  rsl::string_view project_name()
  {
    return internal::project_name();
  }
}
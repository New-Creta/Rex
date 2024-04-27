#pragma once

#include "rex_std/string_view.h"

namespace rex
{
  // This return the name of the project itself. This is used by engine systems (eg. filesystem)
  // for initialization
  bool set_project_name(rsl::string_view projectName);

  rsl::string_view project_name();
}
#pragma once

#include "rex_std/string_view.h"

namespace rex
{
  // This return the name of the project itself. This is used by systems (eg. filesystem)
  // for initialization
  extern rsl::string_view project_name();
}
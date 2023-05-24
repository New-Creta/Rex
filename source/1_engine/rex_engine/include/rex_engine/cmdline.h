#pragma once

#include "rex_std/string_view.h"

namespace rex
{
  namespace cmdline
  {
    struct CommandLineArgument
    {
      rsl::string_view name;
      rsl::string_view desc;
    };

    void init(rsl::string_view cmdLine);

    rsl::optional<rsl::string_view> get_argument(rsl::string_view arg);
  }
}
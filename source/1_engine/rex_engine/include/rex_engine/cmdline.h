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
      rsl::string_view module; // the module this argument is defined for, very useful for debugging as well as for the help command
    };

    void init(rsl::string_view cmdLine);

    rsl::optional<rsl::string_view> get_argument(rsl::string_view arg);
    void print_args();
  } // namespace cmdline
} // namespace rex
#pragma once

#include "rex_std/bonus/string.h"

namespace rex
{
  namespace cmdline
  {
    struct Argument
    {
      rsl::string_view name;
      rsl::string_view desc;
      rsl::string_view module; // the module (eg. RexEngine) this argument is defined for, very useful for debugging as well as for the help command
    };

    void init(rsl::string_view cmdLine);
    void shutdown();
    void print_args();

    // This is used to scan if a certain argument is specified
    rsl::optional<rsl::string_view> get_argument(rsl::string_view arg);

    // This is used to print the entire commandline and should not be used for parsing
    rsl::string_view get();
  } // namespace cmdline
} // namespace rex
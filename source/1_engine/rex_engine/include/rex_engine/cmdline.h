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

    struct CommandLineArgumentsView
    {
      CommandLineArgument* args;
      count_t count;
    };

    void init(rsl::string_view cmdLine, CommandLineArgumentsView engineArgs, CommandLineArgumentsView projectArgs);

    rsl::optional<rsl::string_view> get_argument(rsl::string_view arg);



    // needs to be implemented by the client
    CommandLineArgumentsView receive_cmd_line_args();
  }
}
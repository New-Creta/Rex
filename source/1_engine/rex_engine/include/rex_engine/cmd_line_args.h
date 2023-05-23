#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/vector.h"

namespace rex
{
  struct CommandLineArgument
  {
    rsl::string_view name;
    rsl::string_view desc;
  };

  rsl::array command_line_arguments
  {
    CommandLineArgument{ "AutoTest", "Specify which auto test to run"}
  };

  class CommandLineArguments
  {
  public:
    explicit CommandLineArguments(const char8* cmdLine);

  private:
    void parse_cmd_line(rsl::string_view cmdLine);

  private:
    rsl::vector<rsl::string_view> m_arguments;
  };
} // namespace rex
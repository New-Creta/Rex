#pragma once

#include "rex_std/vector.h"
#include "rex_std/string_view.h"

namespace rex
{
  class CommandLineArguments
  {
  public:
    CommandLineArguments(const char8* cmdLine);

  private:
    void parse_cmd_line(rsl::string_view cmdLine);

  private:
    rsl::vector<rsl::string_view> m_arguments;
  };
}
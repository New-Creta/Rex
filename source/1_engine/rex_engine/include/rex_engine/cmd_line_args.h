#pragma once

#include "rex_std/bonus/types.h" // for char8
#include "rex_std/vector.h" // for string_view, vector

namespace rex
{
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
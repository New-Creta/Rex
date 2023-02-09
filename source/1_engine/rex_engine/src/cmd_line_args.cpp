#include "rex_engine/cmd_line_args.h"

namespace rex
{
  CommandLineArguments::CommandLineArguments(const char8* cmdLine)
  {
    parse_cmd_line(rsl::string_view(cmdLine));
  }

  void CommandLineArguments::parse_cmd_line(rsl::string_view cmdLine)
  {
    // all we have to do is split the command line based with spaces
    // there's no need to perform any heap allocation for that.
    count_t start_pos = cmdLine.find_first_not_of(' ', 0); // it's possible, although unlikely, that we start with a space

    // all executables should be called with at least 1 argument
    // the first argument should always be the path to the executable itself
    REX_ASSERT_X(start_pos != -1, "No arguments given to your exe, there should always be at least 1 arg.");

    count_t space_pos = cmdLine.find_first_of(' ', start_pos);
    while(start_pos != -1 && space_pos != -1)
    {
      const count_t length = space_pos - start_pos;
      m_arguments.push_back(cmdLine.substr(start_pos, length));
      start_pos = cmdLine.find_first_not_of(' ', space_pos); // skip all additional spaces
      space_pos = cmdLine.find_first_of(' ', start_pos);
    }

    if(start_pos != -1)
    {
      m_arguments.push_back(cmdLine.substr(start_pos));
    }
  }

} // namespace rex
#include "rex_engine/entrypoint.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_std/string.h"

namespace rex
{
  void pre_app_entry(const char8* cmdLine)
  {
    const rsl::string_view cmd_line(cmdLine);
    const rsl::string_view root_arg("-root=");
    const count_t root_pos = cmd_line.find(root_arg);

    rsl::string_view root = "";
    if (root_pos != -1)
    {
      count_t space_pos = cmd_line.find_first_of(" ", root_pos);
      space_pos = space_pos == -1 ? cmd_line.length() : space_pos;

      const count_t root_start = root_pos + root_arg.length();
      const count_t root_length = space_pos - root_start;
      root = cmd_line.substr(root_start, root_length);
    }

    vfs::init(root);
  }
}
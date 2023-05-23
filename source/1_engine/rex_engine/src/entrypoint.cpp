#include "rex_engine/entrypoint.h"

#include "rex_std/string.h"

#include "rex_engine/cmd_line_args.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/cmdline.h"
#include "rex_std/array.h"

namespace rex
{
  rsl::array command_line_arguments
  {
    cmdline::CommandLineArgument{ "Root", "Specify the directory to use as root."},
    cmdline::CommandLineArgument{ "AutoTest", "Specify which auto test to run."},
  };

  cmdline::CommandLineArgumentsView receive_engine_cmd_args()
  {
    return { command_line_arguments.data(), command_line_arguments.size() };
  }

  void pre_app_entry(REX_MAYBE_UNUSED const char8* cmdLine) 
  {
    cmdline::CommandLineArgumentsView engine_args_view = receive_engine_cmd_args();
    cmdline::CommandLineArgumentsView args_view = cmdline::receive_cmd_line_args();

    cmdline::init(rsl::string_view(cmdLine), engine_args_view, args_view);

    vfs::init();
  }

  void post_app_shutdown() 
  {

  }
} // namespace rex
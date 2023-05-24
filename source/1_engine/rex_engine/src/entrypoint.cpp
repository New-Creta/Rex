#include "rex_engine/entrypoint.h"

#include "rex_std/string.h"

#include "rex_engine/cmd_line_args.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/cmdline.h"
#include "rex_std/array.h"

namespace rex
{
  void pre_app_entry(REX_MAYBE_UNUSED const char8* cmdLine) 
  {
    cmdline::init(rsl::string_view(cmdLine));

    vfs::init();
  }

  void post_app_shutdown() 
  {

  }
} // namespace rex
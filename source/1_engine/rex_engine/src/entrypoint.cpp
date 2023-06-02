#include "rex_engine/entrypoint.h"

#include "rex_engine/cmdline.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/diagnostics/logging/logger_config.h"
#include "rex_std/array.h"
#include "rex_std/internal/exception/exit.h"
#include "rex_std/string.h"

namespace rex
{
  namespace internal
  {
    void pre_app_entry(REX_MAYBE_UNUSED const char8* cmdLine)
    {
      cmdline::init(rsl::string_view(cmdLine));

      // if a user wants to know the arguments for the executable, we want to perform as minimal setup as possible.
      // we just initialize the commandline, print what's possible and exit the program
      if(cmdline::get_argument("help"))
      {
        cmdline::print_args();
        rsl::exit(0);
      }

      diagnostics::init();
      vfs::init();
    }

    void post_app_shutdown() {}
  } // namespace internal
} // namespace rex
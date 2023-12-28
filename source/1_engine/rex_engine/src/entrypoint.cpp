#include "rex_engine/engine/entrypoint.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/logging/logger_config.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_std/array.h"
#include "rex_std/chrono.h"
#include "rex_std/internal/exception/exit.h"
#include "rex_std/string.h"
#include "rex_std/thread.h"

namespace rex
{
  namespace internal
  {
    void pre_app_entry(REX_MAYBE_UNUSED const tchar* cmdLine)
    {
      cmdline::init(rsl::wstring_view(cmdLine));

      // if a user wants to know the arguments for the executable, we want to perform as minimal setup as possible.
      // we just initialize the commandline, print what's possible and exit the program
      if(cmdline::get_argument(L"help"))
      {
        cmdline::print_args();
        rsl::exit(0);
      }

      // if we want to debug executables without a debugger, we need to break early
      // so we can attach a debugger and continue from then on
      // we'll have a timer in place to break for 2 minutes, if no debugger is attached
      // we close down the program
      if(cmdline::get_argument(L"BreakOnBoot"))
      {
        using namespace rsl::chrono_literals; // NOLINT(google-build-using-namespace)
        auto i = 1s;
        while(i < 10min)
        {
          rsl::this_thread::sleep_for(1s);
          ++i;
        }

        // when the debugger is attached, skip this line
        rsl::exit(0);
      }

      diagnostics::init_log_levels();
      vfs::init();
    }

    void post_app_shutdown()
    {
      vfs::shutdown();

      cmdline::shutdown();
    }
  } // namespace internal
} // namespace rex
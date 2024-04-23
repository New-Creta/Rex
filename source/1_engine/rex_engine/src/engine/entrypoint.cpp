#include "rex_engine/engine/entrypoint.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/debug.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/logging/logger_config.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_std/bonus/attributes.h"
#include "rex_std/internal/exception/exit.h"
#include "rex_std/thread.h"

namespace rex
{
  namespace internal
  {
    void pre_app_entry(RSL_MAYBE_UNUSED const char8* cmdLine)
    {
      cmdline::init(rsl::string_view(cmdLine));

      // if a user wants to know the arguments for the executable, we want to perform as minimal setup as possible.
      // we just initialize the commandline, print what's possible and exit the program
      if(cmdline::get_argument("help"))
      {
        cmdline::print_args();
        rsl::exit(0);
      }

      // if we want to debug executables without a debugger, we need to break early
      // so we can attach a debugger and continue from then on
      // we'll have a timer in place to break for 2 minutes, if no debugger is attached
      // we close down the program
      if(cmdline::get_argument("BreakOnBoot"))
      {
        if (!wait_for_debugger())
        {
          rsl::exit(1); // exit if debugger didn't get attached
        }
      }

      // If the program was spawned without a debugger and we want to automatically attach one
      if(cmdline::get_argument("AttachOnBoot"))
      {
        attach_debugger();
      }

      // Initialize the log levels as early as possible
      // They don't have dependencies (other than the commandline)
      // and are pretty much required by everything else
      diagnostics::init_log_levels();

      // Log early on if any sanitization is enabled
      // This is useful to have in the log file to make sure that correct sanitization is enabled when testing
      log_sanitization();

      // Initialize the filesystem as this can be needed by the entry point of the entrypoint of the client
      // However it is recommended that all initialziation code is moved into the client's init function.
      // If we decide to limit this more aggresively, we can move this initialization to the initialize function
      // of the engine.
      vfs::init();
    }

    void post_app_shutdown()
    {
      vfs::shutdown();

      cmdline::shutdown();
    }
  } // namespace internal
} // namespace rex
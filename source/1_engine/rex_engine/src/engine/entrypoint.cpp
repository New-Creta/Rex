#include "rex_engine/engine/entrypoint.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/debug.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_std/bonus/attributes.h"
#include "rex_std/internal/exception/exit.h"
#include "rex_std/thread.h"

#include "rex_engine/engine/mutable_globals.h"

namespace rex
{
  namespace internal
  {
    void log_pre_init_results()
    {
      // Now log the commandline we started the app with
      cmdline::print();

      // Log early on if any sanitization is enabled
      // This is useful to have in the log file to make sure that correct sanitization is enabled when testing
      log_sanitization();

      REX_INFO(LogEngine, "Vfs Root: {}", rex::vfs::root());
      REX_INFO(LogEngine, "Session Directory: {}", rex::vfs::current_session_root());
      REX_INFO(LogEngine, "Log Path: {}", rex::log_path());
    }

    void pre_app_entry(REX_MAYBE_UNUSED const char8* cmdLine)
    {
      // Initialize the commandline first as this can influence everything else
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
        if(!wait_for_debugger())
        {
          rsl::exit(1); // exit if debugger didn't get attached
        }
      }

      // If the program was spawned without a debugger and we want to automatically attach one
      if(cmdline::get_argument("AttachOnBoot"))
      {
        attach_debugger();
      }

      // Assign a very small buffer to the single frame allocator so it can be used
      // It'll be overwritten later
      rex::mut_globals().single_frame_allocator = rsl::make_unique<FrameBasedAllocator>(1_mib, 1);

      // Initialize the filesystem as this can be needed by the entry point of the client
      // However it is recommended that all initialziation code is moved into the client's init function.
      // If we decide to limit this more aggresively, we can move this initialization to the initialize function
      // of the engine.
      vfs::init();

      // Now initialize all the logging diagnostics, including setting up file output
      // We need to do this here as we need the vfs to be initialized
      // We purposely don't initialize anything else here as this is meant to be a quick initialization phase
      // Proper initialization of other, more adavanced systems is deferred until the initialize function of the app
      log::init();

      // Now that the pre initialization has finished, including logging
      // log what we just did
      log_pre_init_results();
    }

    void post_app_shutdown()
    {
      mut_globals().single_frame_allocator.reset();
    }
  } // namespace internal
} // namespace rex
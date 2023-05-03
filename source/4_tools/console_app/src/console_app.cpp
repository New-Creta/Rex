#include "rex_engine/defines.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/entrypoint.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_std/iostream.h"
#include "rex_std_extra/utility/type_id.h"
#include "rex_windows/console_application.h"
#include "rex_engine/event_system.h"
#include "rex_windows/input/input.h"

DEFINE_LOG_CATEGORY(LogConsoleApp, rex::LogVerbosity::Log);

#include <Windows.h>

namespace rex
{
  bool initialize()
  {
    // the following pointer should get tracked and automatically be processed by the tracking system
    // depending on which one is enabled

    // the code will follow the following step:
    // operator new -> Global Allocator (Tracked) -> Global Allocator (Untracked) -> System Call
    //                                                                                    |
    //                                                                                    | 
    // operator new <- Global Allocator (Tracked) <- Global Allocator (Untracked)  <------+ 
    //                         |
    //                         |
    //                         v
    //                   Save to file
    // 
    // Functionality should be implemented in memory_tracking.cpp
    // However, we'll need a file system first to save things to disk
    // We can then write a tool that scans this file for a memory analysis
    // 
    // Requirements of this tool:
    // - Open a memory report file saved by the engine (See above)
    // - Track all live allocations with their callstacks
    // - Diff between different points in time
    // - Track all single frame allocations


    rsl::unique_ptr<int> p = rsl::make_unique<int>(1);


    return true;
  }
  void update()
  {
   
  }
  void shutdown()
  {
  }

  ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams, CommandLineArguments&& cmdArgs)
  {
    ApplicationCreationParams app_params(rsl::move(platformParams), rsl::move(cmdArgs));

    app_params.engine_params.max_memory        = 256_kb;
    app_params.engine_params.app_init_func     = initialize;
    app_params.engine_params.app_update_func   = update;
    app_params.engine_params.app_shutdown_func = shutdown;

    return app_params;
  }
} // namespace rex

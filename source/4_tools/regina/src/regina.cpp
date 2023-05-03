#include "rex_engine/defines.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/entrypoint.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_std/iostream.h"
#include "rex_std_extra/utility/type_id.h"
#include "rex_windows/gui_application.h"

DEFINE_LOG_CATEGORY(LogRegina, rex::LogVerbosity::Log);

namespace rex
{
  bool initialize()
  {
    REX_LOG(LogRegina, "Initializing Regina");

    return true;
  }
  void update()
  {
    // REX_INFO("updating Regina");
  }
  void shutdown()
  {
    REX_LOG(LogRegina, "shutting down Regina");
  }

  ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams, CommandLineArguments&& cmdArgs)
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
    //               Save to file/Send to tool
    // 
    // Functionality should be implemented in memory_tracking.cpp
    // However, we'll need a file system first to save things to disk
    // Later we can add a network layer to send data over the network to a different tool
    // This tool will use the rex engine as well as a backbone.
    // The tool will open a socket and will listen to requests coming in on that socket
    // 
    // Requirements of this tool:
    // - Open a memory report file saved by the engine (See above)
    // - Track all live allocations with their callstacks
    // - Diff between different points in time
    // - Track all single frame allocations
   

    rsl::unique_ptr<int> p = rsl::make_unique<int>(1);











    ApplicationCreationParams app_params(rsl::move(platformParams), rsl::move(cmdArgs));

    app_params.gui_params.window_width  = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title  = "Regina";

    app_params.engine_params.max_memory        = 256_kb;
    app_params.engine_params.app_init_func     = initialize;
    app_params.engine_params.app_update_func   = update;
    app_params.engine_params.app_shutdown_func = shutdown;
    app_params.create_window = true;

    return app_params;
  }
} // namespace rex

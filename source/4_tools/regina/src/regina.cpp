#include "rex_engine/diagnostics/logging/log_macros.h"     // for REX_LOG
#include "rex_engine/entrypoint.h"                         // for app_entry
#include "rex_engine/string/stringid.h"                    // for operator""...
#include "rex_windows/gui_application.h"                   // for Applicatio...
#include "rex_engine/cmd_line_args.h"                      // for CommandLin...
#include "rex_engine/core_application.h"                   // for EngineParams
#include "rex_engine/diagnostics/logging/log_verbosity.h"  // for LogVerbosity
#include "rex_std/bonus/types.h"                           // for char8
#include "rex_std/type_traits.h"                           // for move
#include "rex_std_extra/memory/memory_size.h"              // for operator""_kb
#include "rex_windows/platform_creation_params.h"          // for PlatformCr...

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
    ApplicationCreationParams app_params(rsl::move(platformParams), rsl::move(cmdArgs));

    app_params.gui_params.window_width  = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title  = "Regina"_sid;

    app_params.engine_params.max_memory        = 256_kb;
    app_params.engine_params.app_init_func     = initialize;
    app_params.engine_params.app_update_func   = update;
    app_params.engine_params.app_shutdown_func = shutdown;

    return app_params;
  }
} // namespace rex

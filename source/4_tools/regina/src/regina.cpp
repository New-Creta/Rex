#include "rex_engine/defines.h"
#include "rex_engine/entrypoint.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/diagnostics/logging.h"
#include "rex_std/iostream.h"
#include "rex_std_extra/utility/type_id.h"
#include "rex_windows/gui_application.h"

namespace rex
{
  bool initialize()
  {
    REX_INFO("Initializing Regina");
    return true;
  }
  void update()
  {
    REX_INFO("updating Regina");
  }
  void shutdown()
  {
    REX_INFO("shutting down Regina");
  }

  ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams, CommandLineArguments&& cmdArgs)
  {
    ApplicationCreationParams app_params(rsl::move(platformParams), rsl::move(cmdArgs));

    app_params.gui_params.window_width  = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title  = "Regina";

    app_params.engine_params.max_memory = 256_kb;
    app_params.engine_params.app_init_func = initialize;
    app_params.engine_params.app_update_func = update;
    app_params.engine_params.app_shutdown_func = shutdown;

    return app_params;
  }
} // namespace rex

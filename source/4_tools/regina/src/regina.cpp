#include "rex_engine/defines.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/entrypoint.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/string/stringid.h"
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
    ApplicationCreationParams app_params(rsl::move(platformParams), rsl::move(cmdArgs));

    app_params.gui_params.window_width  = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title  = "Regina"_sid;

    app_params.engine_params.max_memory        = 256_kb;
    app_params.engine_params.app_init_func     = initialize;
    app_params.engine_params.app_update_func   = update;
    app_params.engine_params.app_shutdown_func = shutdown;
    app_params.create_window = true;

    return app_params;
  }
} // namespace rex

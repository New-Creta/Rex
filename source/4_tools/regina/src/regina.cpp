#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/engine_params.h"
#include "rex_engine/entrypoint.h"
#include "rex_std/string.h"
#include "rex_std_extra/memory/memory_size.h"
#include "rex_windows/platform_creation_params.h"

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

  ApplicationCreationParams create_regina_app_creation_params(PlatformCreationParams&& platformParams)
  {
    ApplicationCreationParams app_params(rsl::move(platformParams));

    app_params.gui_params.window_width  = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title  = "Regina";

    app_params.engine_params.max_memory        = 256_kb;
    app_params.engine_params.app_init_func     = initialize;
    app_params.engine_params.app_update_func   = update;
    app_params.engine_params.app_shutdown_func = shutdown;
    app_params.create_window                   = true;

    return app_params;
  }

#ifndef REX_ENABLE_AUTO_TESTS

  ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams)
  {
    return create_regina_app_creation_params(rsl::move(platformParams));
  }
#endif
} // namespace rex

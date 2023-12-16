#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/engine_params.h"
#include "rex_engine/entrypoint.h"
#include "rex_std/string.h"
#include "rex_std_extra/memory/memory_size.h"
#include "rex_windows/platform_creation_params.h"

DEFINE_LOG_CATEGORY(LogRegina, rex::LogVerbosity::Log);

namespace regina
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

  rex::ApplicationCreationParams create_regina_app_creation_params(rex::PlatformCreationParams&& platformParams)
  {
    rex::ApplicationCreationParams app_params(&platformParams);

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
} // namespace rex

#ifndef REX_ENABLE_AUTO_TESTS
namespace rex
{
  ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams)
  {
    return create_regina_app_creation_params(rsl::move(platformParams));
  }
}
#endif

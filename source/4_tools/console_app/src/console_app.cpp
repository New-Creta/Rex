#include "rex_engine/defines.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/entrypoint.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_std/iostream.h"
#include "rex_std_extra/utility/type_id.h"
#include "rex_windows/console_application.h"

DECLARE_LOG_CATEGORY_EXTERN(LogConsoleApp, LogVerbosity::Log);
DEFINE_LOG_CATEGORY(LogConsoleApp);

#include <Windows.h>

namespace rex
{
  bool initialize()
  {
    return true;
  }
  void update()
  {
    if (GetKeyState('A') & 0x8000)
    {
      REX_LOG(LogConsoleApp, "Updating ConsoleApp");
    }
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

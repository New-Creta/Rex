#include "rex_engine/defines.h"
#include "rex_engine/entrypoint.h"

#include "rex_windows/gui_application.h"

namespace rex
{
  s32 app_entry(const PlatformCreationParams& platformParams, CommandLineArguments&& cmdArgs)
  {
    rex::win32::ApplicationCreationParams app_params;

    app_params.window_width = 1280;
    app_params.window_height = 720;
    app_params.window_title = "Sandbox";

    rex::win32::GuiApplication application(platformParams, app_params, rsl::move(cmdArgs));

    return application.run();
  }
} // namespace rex

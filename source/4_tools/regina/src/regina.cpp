#include "rex_engine/defines.h"
#include "rex_engine/entrypoint.h"
#include "rex_windows/gui_application.h"

namespace rex
{
  ApplicationCreationParams app_entry(const PlatformCreationParams& /*platformParams*/, const CommandLineArguments& /*cmdArgs*/)
  {
    ApplicationCreationParams app_params;

    app_params.window_width  = 1280;
    app_params.window_height = 720;
    app_params.window_title  = "Sandbox";

    return app_params;
  }
} // namespace rex

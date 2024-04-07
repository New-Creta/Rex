#include "rex_engine/engine/entrypoint.h"

namespace rex
{
  ApplicationCreationParams app_entry(PlatformCreationParams& platformParams)
  {
    rex::ApplicationCreationParams app_params(platformParams);

    app_params.gui_params.window_width = 1280;
    app_params.gui_params.window_height = 720;
    app_params.gui_params.window_title = "Pokemon";

    app_params.create_window = true;

    return app_params;
  }
}
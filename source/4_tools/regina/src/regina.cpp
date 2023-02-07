#include "rex_engine/core_application_params.h"
#include "rex_engine/defines.h"
#include "rex_engine/entrypoint.h"

namespace rex
{
  ApplicationCreationParams app_entry(s32 argc, char8** argv)
  {
    UNUSED_PARAM(argc);
    UNUSED_PARAM(argv);

    ApplicationCreationParams params;

    params.window_width  = 1280;
    params.window_height = 720;
    params.window_title  = "Sandbox";

    return params;
  }
} // namespace rex

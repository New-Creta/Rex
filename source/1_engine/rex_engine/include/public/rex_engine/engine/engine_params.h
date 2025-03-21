#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/functional.h"
#include "rex_std/limits.h"

namespace rex
{
  struct ApplicationCreationParams;
  struct EngineParams
  {
    using init_func     = rsl::function<bool(const ApplicationCreationParams& appCreationParams)>;
    using update_func   = rsl::function<void()>;
    using shutdown_func = rsl::function<void()>;

    init_func app_init_func         = nullptr;
    update_func app_update_func     = nullptr;
    shutdown_func app_shutdown_func = nullptr;

    rsl::string app_name = rsl::string("default app name");
  };

  // We need to define these params here so that we can use everywhere
  struct GuiParams
  {
    s32 window_width              = 1280;
    s32 window_height             = 720;
    bool fullscreen               = false;
    rsl::string window_title;     // If empty, it'll use the app name

    s32 max_render_commands  = (rsl::numeric_limits<s16>::max)(); // value taken out of thin air, has no special meaning.
    s32 max_frames_in_flight = 3;
    s32 max_fps              = 60;
  };

  struct PlatformCreationParams;
  struct ApplicationCreationParams
  {
  public:
    explicit ApplicationCreationParams(PlatformCreationParams& platformParams)
        : engine_params()
        , platform_params(&platformParams)
        , gui_params()
        , is_gui_app(false)
    {
    }

  public:
    EngineParams engine_params;
    PlatformCreationParams* platform_params;
    GuiParams gui_params;
    bool is_gui_app;
  };
} // namespace rex
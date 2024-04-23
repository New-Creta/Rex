#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/functional.h"
#include "rex_std/limits.h"

namespace rex
{
  struct EngineParams
  {
    using init_func     = rsl::function<bool()>;
    using update_func   = rsl::function<void()>;
    using shutdown_func = rsl::function<void()>;

    init_func app_init_func         = nullptr;
    update_func app_update_func     = nullptr;
    update_func app_gui_func       = nullptr;
    shutdown_func app_shutdown_func = nullptr;
  };

  // We need to define these params here so that we can use everywhere
  struct GuiParams
  {
    s32 window_width              = 1280;
    s32 window_height             = 720;
    rsl::string_view window_title = "Application";
    bool fullscreen               = false;

    s32 max_fps              = 60;
  };

  struct PlatformCreationParams;
  struct ApplicationCreationParams
  {
  public:
    explicit ApplicationCreationParams(PlatformCreationParams* platformParams)
        : engine_params()
        , platform_params(platformParams)
        , gui_params()
        , create_window(false)
    {
    }

  public:
    EngineParams engine_params;
    PlatformCreationParams* platform_params;
    GuiParams gui_params;
    bool create_window;
  };
} // namespace rex
#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/functional.h"
#include "rex_std/limits.h"
#include "rex_std/bonus/memory/memory_size.h"

namespace rex
{
  struct EngineParams
  {
    using init_func     = rsl::function<bool()>;
    using update_func   = rsl::function<void()>;
    using shutdown_func = rsl::function<void()>;

    // how much memory is the entire app allowed to use.
    // by default, there's no limit on this.
    rsl::memory_size max_memory     = rsl::memory_size((rsl::numeric_limits<s64>::max)());
    init_func app_init_func         = nullptr;
    update_func app_update_func     = nullptr;
    shutdown_func app_shutdown_func = nullptr;
  };

  // We need to define these params here so that we can use everywhere
  struct GuiParams
  {
    s32 window_width = 1280;
    s32 window_height = 720;
    rsl::string_view window_title = "Application";

    s32 max_render_commands = 1 << 16;
    s32 max_fps = 60;
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
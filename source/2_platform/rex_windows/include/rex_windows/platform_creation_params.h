#pragma once

#include "rex_engine/engine_params.h"
#include "rex_engine/string/stringid.h"
#include "rex_windows/win_types.h"

namespace rex
{
  // can't be in a namespace as they're forward declared in rex namespace
  // the linker needs to think this is in rex namespace.

  struct PlatformCreationParams
  {
    rex::win32::HInstance instance;
    rex::win32::HInstance prev_instance;
    s32 show_cmd;
  };

  struct GuiParams
  {
    s32 window_width              = 1280;
    s32 window_height             = 720;
    rsl::string_view window_title = "Application";
    bool fullscreen               = false;

    s32 max_render_commands       = rsl::numeric_limits<s16>::max();
    s32 max_frames_in_flight      = 3;
    s32 max_fps                   = 60;
  };

  struct ApplicationCreationParams
  {
  public:
    explicit ApplicationCreationParams(PlatformCreationParams&& platformParams)
        : engine_params()
        , platform_params(rsl::move(platformParams))
        , gui_params()
        , create_window(false)
    {
    }

  public:
    EngineParams engine_params;
    PlatformCreationParams platform_params;
    GuiParams gui_params;
    bool create_window;
  };
} // namespace rex
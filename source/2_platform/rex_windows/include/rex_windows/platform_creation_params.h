#pragma once

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
    rex::win32::LPtStr cmd_line;
    s32 show_cmd;
  };

  struct GuiParams
  {
    s32 window_width      = 1280;
    s32 window_height     = 720;
    StringID window_title = "Application"_sid;

    s32 max_render_commands = 1 << 16;
    s32 max_fps             = 60;
  };

  struct ApplicationCreationParams
  {
  public:
    ApplicationCreationParams(PlatformCreationParams&& platformParams, CommandLineArguments&& cmdArgs)
        : engine_params()
        , platform_params(rsl::move(platformParams))
        , gui_params()
        , cmd_args(rsl::move(cmdArgs))
        , create_window(false)
    {
    }

  public:
    EngineParams engine_params;
    PlatformCreationParams platform_params;
    GuiParams gui_params;
    CommandLineArguments cmd_args;
    bool create_window;
  };
} // namespace rex
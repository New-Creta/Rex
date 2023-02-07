#pragma once

#include "rex_engine/types.h"

namespace rex
{
  inline void ThisIsATestFunc()
  {}

  struct ApplicationCreationParams
  {
    s32 window_width          = 1280;
    s32 window_height         = 720;
    const char8* window_title = "Application";

    s32 max_render_commands = 1 << 16;
    s32 max_fps             = 60;
  };
} // namespace rex
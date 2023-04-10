#pragma once

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
} // namespace rex 
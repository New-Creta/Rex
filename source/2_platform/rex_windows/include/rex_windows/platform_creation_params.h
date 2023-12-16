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
} // namespace rex
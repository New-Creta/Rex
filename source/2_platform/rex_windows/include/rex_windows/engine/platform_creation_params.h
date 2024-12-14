#pragma once

#include "rex_engine/engine/engine_params.h"
#include "rex_engine/string/stringid.h"
#include "rex_engine/platform/win/win_types.h"

namespace rex
{
  // can't be in a namespace as they're forward declared in rex namespace
  // the linker needs to think this is in rex namespace.

  struct PlatformCreationParams
  {
    rex::win::HInstance instance;
    rex::win::HInstance prev_instance;
    s32 show_cmd;
  };

} // namespace rex
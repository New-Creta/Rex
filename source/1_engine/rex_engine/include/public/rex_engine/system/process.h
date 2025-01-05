#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  u32 current_process_id();
}

#ifdef REX_PLATFORM_WINDOWS
  #include "rex_engine/platform/win/system/win_process.h"
#endif
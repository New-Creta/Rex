#pragma once

#include "rex_engine/platform/win/win_types.h"

#include "rex_std/bonus/platform/windows/handle.h"

namespace rex
{
  namespace win
  {
    rsl::win::handle_t current_process();
    DWord current_process_id();
  }
}
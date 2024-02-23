#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/platform/win/win_types.h"
#include "rex_std/bonus/platform/windows/handle.h"

namespace rex
{
  rsl::win::handle_t current_process();
  u32 current_process_id();
} // namespace rex
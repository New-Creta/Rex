#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/platform/win/win_types.h"
#include "rex_std/bonus/platform/windows/handle.h"

#include "rex_engine/system/process.h"

namespace rex
{
  rsl::win::handle_t current_process();
} // namespace rex
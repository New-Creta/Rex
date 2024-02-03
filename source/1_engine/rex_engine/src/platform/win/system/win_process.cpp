#include "rex_engine/platform/win/system/win_process.h"

#include <Windows.h>

namespace rex
{
  rsl::win::handle_t current_process()
  {
    return GetCurrentProcess();
  }
  u32 current_process_id()
  {
    return static_cast<u32>(GetProcessId(current_process()));
  }
} // namespace rex
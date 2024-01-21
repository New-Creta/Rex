#include "rex_engine/platform/win/process.h"

#include <Windows.h>

namespace rex
{
  namespace win
  {
    rsl::win::handle_t current_process()
    {
      return GetCurrentProcess();
    }
    DWord current_process_id()
    {
      return GetProcessId(current_process());
    }
  }
}
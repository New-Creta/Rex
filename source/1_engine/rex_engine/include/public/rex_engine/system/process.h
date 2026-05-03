#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/memory/memory_types.h"

namespace rex
{
  struct RunProcessResult
  {
    rsl::string output;
    s32 returncode;
  };

  namespace process
  {
    RunProcessResult run(rsl::string_view cmd);
  }

  namespace current_process
  {
		u32 id();
    scratch_string path();
    void path(char8* buffer, s32 length);
  }
}

#ifdef REX_PLATFORM_WINDOWS
  #include "rex_engine/platform/win/system/win_process.h"
#endif
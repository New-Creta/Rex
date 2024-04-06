#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace win
  {
    s32 report_crash_from_main_thread(void* exceptionInfo);
    s32 report_crash_from_thread(void* exceptionInfo);
  } // namespace win
} // namespace rex
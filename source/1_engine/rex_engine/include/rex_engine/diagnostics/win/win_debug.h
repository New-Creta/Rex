#pragma once

#include "rex_std/bonus/types.h"

namespace rex::win
{
  bool is_debugger_attached();
  void exit_process(int32 exitCode);
  void exit_confirm(int32 exitCode);
} // namespace rex::win
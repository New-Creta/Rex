// ============================================
//
// REX GAME ENGINE
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: win_debug.cpp
// Copyright (c) Nick De Breuck 2023
//
// ============================================

#include "rex_engine/diagnostics/win/win_debug.h"
#include "rex_engine/diagnostics/win/win_message_box.h"

#define NOMINMAX
#include <Windows.h>

bool rex::win::is_debugger_attached()
{
  return IsDebuggerPresent() > 0;
}
void rex::win::exit_process(int32 exit_code)
{
  TerminateProcess(GetCurrentProcess(), exit_code);
}
void rex::win::exit_confirm(int32 exit_code)
{
  if (yes_no_message_box("Are you sure you want to abort the process?"))
  {
    exit_process(exit_code);
  }
}
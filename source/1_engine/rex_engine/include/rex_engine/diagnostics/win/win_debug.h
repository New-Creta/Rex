// ============================================
//
// REX GAME ENGINE
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: win_debug.h
// Copyright (c) Nick De Breuck 2023
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/bonus/string.h"
#include "rex_engine/diagnostics/win/win_types.h"

namespace rex::win
{
  bool is_debugger_attached();
  void exit_process(int32 exitCode);
  void exit_confirm(int32 exitCode);

  rsl::medium_stack_string report_win_error(HResult hr, rsl::string_view winFunc, rsl::string_view file, rsl::string_view function, card32 lineNr);
} // namespace rex::win
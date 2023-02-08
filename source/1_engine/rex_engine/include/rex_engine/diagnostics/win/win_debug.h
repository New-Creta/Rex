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

namespace rex::win
{
  bool is_debugger_attached();
  void exit_process(int32 exitCode);
  void exit_confirm(int32 exitCode);
} // namespace rex::win
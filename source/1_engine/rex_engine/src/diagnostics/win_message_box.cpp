// ============================================
//
// REX GAME ENGINE
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: win_message_box.cpp
// Copyright (c) Nick De Breuck 2023
//
// ============================================

#include "rex_engine/diagnostics/win/win_message_box.h"

#include "rex_std/bonus/string/stack_string.h"

#define NOMINMAX
#include <windows.h>

int32 rex::win::message_box(const rsl::string_view msg)
{
  rsl::medium_stack_string title;
  GetModuleFileName(NULL, title.data(), static_cast<DWORD>(title.max_size()));
  title.reset_null_termination_offset();
  return message_box(msg, title);
}
int32 rex::win::message_box(const rsl::string_view msg, const rsl::string_view title)
{
  // We provide NULL as owner so it won't get in the way when we want to view the game window
  // return value not handled, it should break infinitely or crash when running without debugger
  return MessageBox(NULL, msg.data(), title.data(), MB_OK | MB_ICONERROR);
}

bool rex::win::yes_no_message_box(const rsl::string_view msg)
{
  rsl::medium_stack_string title;
  GetModuleFileName(NULL, title.data(), static_cast<DWORD>(title.max_size()));
  title.reset_null_termination_offset();
  return yes_no_message_box(msg, title);
}
bool rex::win::yes_no_message_box(const rsl::string_view msg, const rsl::string_view title)
{
  // We provide NULL as owner so it won't get in the way when we want to view the game window
  // return value not handled, it should break infinitely or crash when running without debugger
  return MessageBox(NULL, msg.data(), title.data(), MB_YESNO) == IDYES;
}
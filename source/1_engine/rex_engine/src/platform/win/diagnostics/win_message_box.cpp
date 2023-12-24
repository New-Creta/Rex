#include "rex_engine/platform/win/diagnostics/win_message_box.h"

#include "rex_std/bonus/string.h"

#define NOMINMAX
#include <Windows.h>

int32 rex::win::message_box(rsl::string_view msg)
{
  rsl::medium_stack_string title;
  GetModuleFileName(nullptr, title.data(), static_cast<DWORD>(title.max_size())); // NOLINT(readability-static-accessed-through-instance)
  title.reset_null_termination_offset();
  return message_box(msg, title);
}
int32 rex::win::message_box(rsl::string_view msg, rsl::string_view title)
{
  // We provide nullptr as owner so it won't get in the way when we want to view the game window
  // return value not handled, it should break infinitely or crash when running without debugger
  return MessageBox(nullptr, msg.data(), title.data(), MB_OK | MB_ICONERROR);
}

bool rex::win::yes_no_message_box(rsl::string_view msg)
{
  rsl::medium_stack_string title;
  GetModuleFileName(nullptr, title.data(), static_cast<DWORD>(title.max_size())); // NOLINT(readability-static-accessed-through-instance)
  title.reset_null_termination_offset();
  return yes_no_message_box(msg, title);
}
bool rex::win::yes_no_message_box(rsl::string_view msg, rsl::string_view title)
{
  // We provide nullptr as owner so it won't get in the way when we want to view the game window
  // return value not handled, it should break infinitely or crash when running without debugger
  return MessageBox(nullptr, msg.data(), title.data(), MB_YESNO) == IDYES;
}
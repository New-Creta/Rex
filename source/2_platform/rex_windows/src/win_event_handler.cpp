#include "rex_windows/win_event_handler.h"

#include "rex_engine/diagnostics/win/win_call.h"
#include "rex_engine/event_system.h"
#include "rex_std/bonus/utility/scopeguard.h"
#include "rex_windows/log.h"

#include "rex_std/fstream.h"
#include "rex_engine/memory/memory_tracking.h"

#include "rex_std/type_traits.h"

#define NOMINMAX
#include <Windows.h>

//-------------------------------------------------------------------------
rex::win32::LResult rex::win32::EventHandler::on_event(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam) // NOLINT (readability-convert-member-functions-to-static,-warnings-as-errors)
{
  // Sometimes Windows set error states between messages
  // becasue these aren't our fault, we'll ignore those
  // to make sure our messages are successful
  const DWORD last_windows_error = GetLastError();
  rex::win::clear_win_errors();

  const rsl::scopeguard reset_win_error_scopeguard([=]() { SetLastError(last_windows_error); });

  switch (msg)
  {
  case WM_CHAR:
    if (wparam == 'w')
    {
      REX_MEM_TAG_SCOPE(MemoryTag::Windows);
      [[maybe_unused]] int* p = new int(2);
    }
    else if (wparam == 'e')
    {
      const MemoryStats mem_stats = rex::mem_tracker().current_stats();

      for (const auto mem_tag : rsl::enum_refl::enum_entries<MemoryTag>())
      {
        REX_LOG(LogWindows, "{} : {} bytes", mem_tag.name(), mem_stats.usage_per_tag[rsl::enum_refl::enum_integer(mem_tag.val())]);
      }

      for (const MemoryHeader* header : mem_stats.allocation_headers)
      {
        REX_LOG(LogWindows, "ptr - {}", header->ptr());
        REX_LOG(LogWindows, "size - {}", header->size());
        REX_LOG(LogWindows, "tag - {}", rsl::enum_refl::enum_name(header->tag()));
        REX_LOG(LogWindows, "thread id - {}", header->thread_id());
        REX_LOG(LogWindows, "frame index - {}", header->frame_index());
        REX_LOG(LogWindows, "callstack");
        REX_LOG(LogWindows, header->callstack());
      }
    }
    break;
  case WM_CLOSE: REX_WARN(LogWindows, "Verify if the user really wants to close"); break;
  case WM_DESTROY:
    PostQuitMessage(0);
    event_system::fire_event(event_system::EventType::WindowClose);
    return 0;
  default:
    // Nothing to implement
    break;
  }
  return DefWindowProc(static_cast<HWND>(hwnd), msg, wparam, lparam);
}

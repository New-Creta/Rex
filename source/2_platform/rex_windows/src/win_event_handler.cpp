#include "rex_windows/win_event_handler.h"

#include "rex_engine/diagnostics/logging.h"
#include "rex_engine/diagnostics/win/win_call.h"
#include "rex_engine/event_system.h"
#include "rex_std/bonus/utility/scopeguard.h"

#define NOMINMAX
#include <Windows.h>

//-------------------------------------------------------------------------
rex::win32::LResult rex::win32::EventHandler::on_event(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam) //NOLINT (readability-convert-member-functions-to-static,-warnings-as-errors)
{
  // Sometimes Windows set error states between messages
  // becasue these aren't our fault, we'll ignore those
  // to make sure our messages are successful
  const DWORD last_windows_error = GetLastError();
  rex::win::clear_win_errors();

  const rsl::scopeguard reset_win_error_scopeguard([=]() { SetLastError(last_windows_error); });

  switch(msg)
  {
    case WM_CLOSE: REX_TODO("Verify if the user really wants to close"); break;
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

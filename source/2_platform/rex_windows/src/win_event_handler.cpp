#include "rex_windows/win_event_handler.h"

#include "rex_engine/core_window.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/win/win_call.h"
#include "rex_engine/event.h"
#include "rex_engine/event_system.h"
#include "rex_engine/event_type.h"
#include "rex_std/bonus/utility.h"
#include "rex_windows/log.h"

#define NOMINMAX
#include <Windows.h>

// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)

namespace rex
{
  namespace win32
  {
    //-------------------------------------------------------------------------
    EventHandler::EventHandler(IWindow* wnd)
        : m_wnd(wnd)
    {
    }

    //-------------------------------------------------------------------------
    LResult EventHandler::on_event(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam) // NOLINT (readability-convert-member-functions-to-static,-warnings-as-errors)
    {
      REX_ASSERT_X(m_wnd != nullptr, "Window was not given to the Window Event Handler");

      // Sometimes Windows set error states between messages
      // because these aren't our fault, we'll ignore those
      // to make sure our messages are successful
      const DWORD last_windows_error = GetLastError();
      rex::win::clear_win_errors();

      const rsl::scopeguard reset_win_error_scopeguard([=]() { SetLastError(last_windows_error); });

      event_system::Event evt = {};

      switch(msg)
      {
        case WM_CLOSE: REX_WARN(LogWindows, "Verify if the user really wants to close"); break;
        case WM_DESTROY:
          PostQuitMessage(0);

          evt.type = event_system::EventType::WindowClose;
          event_system::fire_event(evt);
          return 0;

        case WM_ACTIVATE:
          if(LOWORD(wparam) == WA_INACTIVE)
          {
            evt.type = event_system::EventType::WindowDeactivate;
            event_system::fire_event(evt);
          }
          else
          {
            evt.type = event_system::EventType::WindowActivate;
            event_system::fire_event(evt);
          }
          return 0;

        case WM_ENTERSIZEMOVE:
          evt.type = event_system::EventType::WindowStartWindowResize;
          event_system::fire_event(evt);
          return 0;

        case WM_EXITSIZEMOVE:
          evt.type = event_system::EventType::WindowStopWindowResize;
          event_system::fire_event(evt);
          return 0;

        case WM_MENUCHAR:
          // The WM_MENUCHAR message is sent when a menu is active and the user presses
          // a key that does not correspond to any mnemonic or accelerator key.
          //
          // In other words, don't beep when we alt-enter.
          return MAKELRESULT(0, MNC_CLOSE);

        case WM_GETMINMAXINFO:
          // We catch this message to prevent the window from becoming too small.
          ((MINMAXINFO*)lparam)->ptMinTrackSize.x = m_wnd->min_width();  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting, performance-no-int-to-ptr)
          ((MINMAXINFO*)lparam)->ptMinTrackSize.y = m_wnd->min_height(); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting, performance-no-int-to-ptr)
          return 0;

        case WM_SIZE:
          evt.data.window_resize.window_width  = LOWORD(lparam);
          evt.data.window_resize.window_height = HIWORD(lparam);

          if(wparam == SIZE_MINIMIZED)
          {
            evt.type = event_system::EventType::WindowMinimized;
            event_system::fire_event(evt);
          }
          else if(wparam == SIZE_MAXIMIZED)
          {
            evt.type = event_system::EventType::WindowMaximized;
            event_system::fire_event(evt);
          }
          else if(wparam == SIZE_RESTORED)
          {
            evt.type = event_system::EventType::WindowRestored;
            event_system::fire_event(evt);
          }
          return 0;
        default:
          // Nothing to implement
          break;
      }
      return DefWindowProc(static_cast<HWND>(hwnd), msg, wparam, lparam);
    }

  } // namespace win32
} // namespace rex

// NOLINTEND(cppcoreguidelines-pro-type-union-access)
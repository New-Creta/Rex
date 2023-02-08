#include "rex_windows/win_window.h"

#include "rex_engine/diagnostics/logging.h"
#include "rex_engine/diagnostics/win/win_call.h"
#include "rex_engine/event_system.h"
#include "rex_std/bonus/utility/scopeguard.h"

#define NOMINMAX
#include <Windows.h>

namespace rex
{
  namespace win32
  {
    //-----------------------------------------------------------------
    LResult __stdcall default_win_procedure(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam)
    {
      HWND win_hwnd = reinterpret_cast<HWND>(hwnd);
      if(msg == WM_CREATE)
      {
        CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
        SetWindowLongPtr(win_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create_struct->lpCreateParams));
      }
      else
      {
        Window* this_window = reinterpret_cast<Window*>(GetWindowLongPtrW(win_hwnd, GWLP_USERDATA));
        if(this_window)
        {
          return this_window->on_event(hwnd, msg, wparam, lparam);
        }
      }

      return DefWindowProc(win_hwnd, msg, wparam, lparam);
    }

    //-------------------------------------------------------------------------
    Window::Window()
        : m_wnd_class()
        , m_hwnd(NULL)
        , m_destroyed(false)
    {
    }

    //-------------------------------------------------------------------------
    bool Window::create(HInstance hInstance, s32 cmdShow, const WindowDescription& description)
    {
      if(!m_wnd_class.create(hInstance, default_win_procedure, description.title))
      {
        REX_ERROR("Failed to create window class");
        return false;
      }

      s32 x      = description.viewport.x;
      s32 y      = description.viewport.y;
      s32 width  = description.viewport.width;
      s32 height = description.viewport.height;

      RECT rc = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
      AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

      RECT desktop_rect;
      GetClientRect(GetDesktopWindow(), &desktop_rect);

      LONG screen_mid_x = (desktop_rect.right - desktop_rect.left) / 2;
      LONG screen_mid_y = (desktop_rect.bottom - desktop_rect.top) / 2;

      LONG half_x = (rc.right - rc.left) / 2;
      LONG half_y = (rc.bottom - rc.top) / 2;

      m_hwnd = WIN_CALL(
          (HWND)CreateWindowA(description.title, description.title, WS_OVERLAPPEDWINDOW, x == 0 ? screen_mid_x - half_x : x, y == 0 ? screen_mid_y - half_y : y, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, (HINSTANCE)hInstance, this));

      if(m_hwnd == NULL)
      {
        REX_ERROR("Window creation failed");
        return false;
      }

      ShowWindow(static_cast<HWND>(m_hwnd), cmdShow);
      SetForegroundWindow(static_cast<HWND>(m_hwnd));

      return true;
    }

    //-------------------------------------------------------------------------
    bool Window::destroy()
    {
      if(m_destroyed == false)
      {
        DestroyWindow(static_cast<HWND>(m_hwnd));

        m_wnd_class.destroy();
      }
      else
      {
        REX_WARN("Window already destroyed");
        return false;
      }

      m_destroyed = true;
      return true;
    }

    //-------------------------------------------------------------------------
    void Window::update()
    {
      MSG message = {0};
      while(PeekMessage(&message, NULL, NULL, NULL, PM_REMOVE) > 0)
      {
        TranslateMessage(&message);
        DispatchMessage(&message);
      }
    }
    //-----------------------------------------------------------------
    void Window::show()
    {
      ShowWindow(static_cast<HWND>(m_hwnd), SW_SHOW);
    }
    //-----------------------------------------------------------------
    void Window::hide()
    {
      CloseWindow(static_cast<HWND>(m_hwnd));
    }
    //-----------------------------------------------------------------
    void Window::close()
    {
      destroy();
    }

    //-------------------------------------------------------------------------
    void* Window::get_primary_display_handle()
    {
      return m_hwnd;
    }

    //-------------------------------------------------------------------------
    s32 Window::width() const
    {
      RECT r;
      GetWindowRect(static_cast<HWND>(m_hwnd), &r);

      return r.right - r.left;
    }
    //-------------------------------------------------------------------------
    s32 Window::height() const
    {
      RECT r;
      GetWindowRect(static_cast<HWND>(m_hwnd), &r);

      return r.bottom - r.top;
    }

    //-------------------------------------------------------------------------
    f32 Window::get_aspect() const
    {
      s32 w = width();
      s32 h = height();

      return static_cast<f32>(w) / static_cast<f32>(h);
    }

    //-----------------------------------------------------------------
    LResult Window::on_event(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam)
    {
      // Sometimes Windows set error states between messages
      // becasue these aren't our fault, we'll ignore those
      // to make sure our messages are successful
      const DWORD last_windows_error = GetLastError();
      rex::win::clear_win_errors();

      const rsl::scopeguard reset_win_error_scopeguard([=]() { SetLastError(last_windows_error); });

      switch(msg)
      {
        case WM_CLOSE: close();
        case WM_DESTROY:
          PostQuitMessage(0);
          event_system::fire_event(event_system::EventType::WindowClose);
          return 0;

        default: return DefWindowProc((HWND)hwnd, msg, wparam, lparam);
      }
    }
  } // namespace win32
} // namespace rex
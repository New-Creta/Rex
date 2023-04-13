#include "rex_windows/win_window.h"

#include "rex_engine/diagnostics/win/win_call.h"
#include "rex_std/bonus/utility/scopeguard.h"
#include "rex_windows/log.h"

#define NOMINMAX
#include <Windows.h>

namespace rex
{
  namespace win32
  {
    //-----------------------------------------------------------------
    LResult __stdcall default_win_procedure(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam)
    {
      HWND win_hwnd = static_cast<HWND>(hwnd);
      if(msg == WM_CREATE)
      {
        CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);                                // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
        SetWindowLongPtr(win_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create_struct->lpCreateParams)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
      }
      else
      {
        EventHandler* event_handler = reinterpret_cast<EventHandler*>(GetWindowLongPtrW(win_hwnd, GWLP_USERDATA)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
        if(event_handler)
        {
          return event_handler->on_event(hwnd, msg, wparam, lparam);
        }
      }

      return DefWindowProc(win_hwnd, msg, wparam, lparam);
    }

    //-------------------------------------------------------------------------
    Window::Window()
        : m_wnd_class()
        , m_hwnd(nullptr)
        , m_destroyed(false)
    {
    }

    //-------------------------------------------------------------------------
    bool Window::create(HInstance hInstance, s32 cmdShow, const WindowDescription& description)
    {
      if(!m_wnd_class.create(hInstance, default_win_procedure, description.title))
      {
        REX_ERROR(LogWindows, "Failed to create window class");
        return false;
      }

      const s32 x      = description.viewport.x;
      const s32 y      = description.viewport.y;
      const s32 width  = description.viewport.width;
      const s32 height = description.viewport.height;

      RECT rc = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
      AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

      RECT desktop_rect;
      GetClientRect(GetDesktopWindow(), &desktop_rect);

      LONG screen_mid_x = (desktop_rect.right - desktop_rect.left) / 2;
      LONG screen_mid_y = (desktop_rect.bottom - desktop_rect.top) / 2;

      LONG half_x = (rc.right - rc.left) / 2;
      LONG half_y = (rc.bottom - rc.top) / 2;

      // clang-format off
      m_hwnd = WIN_CALL(
          static_cast<HWND>(CreateWindowA(description.title, 
                                          description.title, 
                                          WS_OVERLAPPEDWINDOW, 
                                          x == 0
                                            ? screen_mid_x - half_x 
                                            : x,
                                          y == 0 
                                            ? screen_mid_y - half_y 
                                            : y, rc.right - rc.left, 
                                          rc.bottom - rc.top, 
                                          nullptr, 
                                          nullptr, 
                                          static_cast<HINSTANCE>(hInstance), 
                                          &m_event_handler)));
      // clang-format on

      if(m_hwnd == nullptr)
      {
        REX_ERROR(LogWindows, "Window creation failed");
        return false;
      }

      ShowWindow(static_cast<HWND>(m_hwnd), cmdShow);
      SetForegroundWindow(static_cast<HWND>(m_hwnd));

      return true;
    }

    //-------------------------------------------------------------------------
    void Window::update()
    {
      MSG message = {};
      while(PeekMessage(&message, nullptr, NULL, NULL, PM_REMOVE) > 0)
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
    void* Window::primary_display_handle()
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
    f32 Window::aspect() const
    {
      const s32 w = width();
      const s32 h = height();

      return static_cast<f32>(w) / static_cast<f32>(h);
    }

    //-------------------------------------------------------------------------
    bool Window::destroy()
    {
      if(m_destroyed == false) // NOLINT(readability-simplify-boolean-expr)
      {
        DestroyWindow(static_cast<HWND>(m_hwnd));

        m_wnd_class.destroy();
      }
      else
      {
        REX_WARN(LogWindows, "Window already destroyed");
        return false;
      }

      m_destroyed = true;
      return true;
    }
  } // namespace win32
} // namespace rex
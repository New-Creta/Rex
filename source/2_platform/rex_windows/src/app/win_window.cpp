#include "rex_windows/app/win_window.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_std/bonus/types.h"
#include "rex_std/string.h"
#include "rex_windows/diagnostics/log.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <cstddef>

namespace rex
{
  namespace win
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
        , m_event_handler(this)
        , m_hwnd(nullptr)
        , m_min_width(200)  // 200 px is a good minimum width for most windows
        , m_min_height(200) // 200 ox is a good minimum height for most windows
        , m_window_state(WindowState::Idle)
    {
    }

    //-------------------------------------------------------------------------
    bool Window::create(HInstance hInstance, s32 cmdShow, const WindowDescription& description)
    {
      UNUSED_PARAM(cmdShow);

      if(!m_wnd_class.create(hInstance, default_win_procedure, description.title.data()))
      {
        REX_ERROR(LogWindows, "Failed to create window class");
        return false;
      }

      m_min_width  = description.min_width;
      m_min_height = description.min_height;

      WindowViewport viewport = description.viewport;

      const s32 x      = viewport.x;
      const s32 y      = viewport.y;
      const s32 width  = viewport.width;
      const s32 height = viewport.height;

      REX_ASSERT_X(width < (1 << 16), "Window width exceeded the maximum resolution");
      REX_ASSERT_X(height < (1 << 16), "Window height exceeded the maximum resolution");

      if(width < m_min_width)
      {
        viewport.width = m_min_width;
      }
      if(height < m_min_height)
      {
        viewport.height = m_min_height;
      }

      RECT rc = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
      if(WIN_FAILED(AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE)))
      {
        REX_ERROR(LogWindows, "Failed to adjust the window rect");
        return false;
      }

      // We use SystemMetrics here instead of retrieving the ClientRect of the DesktopWindow.
      // We might not be able to access the DesktopWindow on this device due to security issues but we are able to retrieve the width and the height of the monitor using SystemMetrics
      const RECT desktop_rect = {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};

      const LONG screen_mid_x = (desktop_rect.right - desktop_rect.left) / 2;
      const LONG screen_mid_y = (desktop_rect.bottom - desktop_rect.top) / 2;

      const LONG half_x = (rc.right - rc.left) / 2;
      const LONG half_y = (rc.bottom - rc.top) / 2;

      // clang-format off
        m_hwnd = WIN_CALL(
            static_cast<HWND>(CreateWindowA(description.title.data(),
                description.title.data(),
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

      return true;
    }

    //-------------------------------------------------------------------------
    void Window::poll_events()
    {
      MSG message = {};
      if(PeekMessage(&message, nullptr, NULL, NULL, PM_REMOVE) > 0)
      {
        TranslateMessage(&message);
        DispatchMessage(&message);
      }
    }
    //-----------------------------------------------------------------
    void Window::show()
    {
      ShowWindow(static_cast<HWND>(m_hwnd), SW_SHOW);
      SetForegroundWindow(static_cast<HWND>(m_hwnd));
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
    s32 Window::min_width() const
    {
      return m_min_width;
    }

    //-------------------------------------------------------------------------
    s32 Window::height() const
    {
      RECT r;
      GetWindowRect(static_cast<HWND>(m_hwnd), &r);

      return r.bottom - r.top;
    }

    //-------------------------------------------------------------------------
    s32 Window::min_height() const
    {
      return m_min_height;
    }

    //-------------------------------------------------------------------------
    f32 Window::aspect() const
    {
      const s32 w = width();
      const s32 h = height();

      return static_cast<f32>(w) / static_cast<f32>(h);
    }

    //-------------------------------------------------------------------------
    void Window::start_resize()
    {
      m_window_state.add_state(WindowState::Resizing);
    }

    //-------------------------------------------------------------------------
    void Window::stop_resize()
    {
      m_window_state.remove_state(WindowState::Resizing);
    }

    //-------------------------------------------------------------------------
    void Window::minimize()
    {
      m_window_state.add_state(WindowState::Minimized);
      m_window_state.remove_state(WindowState::Maximized);
    }

    //-------------------------------------------------------------------------
    void Window::maximize()
    {
      m_window_state.add_state(WindowState::Maximized);
      m_window_state.remove_state(WindowState::Minimized);
    }

    //-------------------------------------------------------------------------
    void Window::restore()
    {
      m_window_state.remove_state(WindowState::Minimized);
      m_window_state.remove_state(WindowState::Maximized);
    }

    //-------------------------------------------------------------------------
    bool Window::is_resizing() const
    {
      return m_window_state.has_state(WindowState::Resizing);
    }

    //-------------------------------------------------------------------------
    bool Window::is_minimized() const
    {
      return m_window_state.has_state(WindowState::Minimized);
    }

    //-------------------------------------------------------------------------
    bool Window::is_maximized() const
    {
      return m_window_state.has_state(WindowState::Maximized);
    }

    //-------------------------------------------------------------------------
    bool Window::destroy()
    {
      if(m_window_state.has_state(WindowState::Destroyed)) // NOLINT(readability-simplify-boolean-expr)
      {
        DestroyWindow(static_cast<HWND>(m_hwnd));

        m_wnd_class.destroy();
      }
      else
      {
        REX_WARN(LogWindows, "Window already destroyed");
        return false;
      }

      m_window_state.add_state(WindowState::Destroyed);
      return true;
    }
  } // namespace win
} // namespace rex
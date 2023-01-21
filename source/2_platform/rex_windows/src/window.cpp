#include "rex_windows/window.h"

#include "rex_windows/internal/win_window_class.h"

#include "rex_engine/event_system.h"

#include "rex_std_extra/utilities/scopeguard.h"

#include "rex_std_extra/math/point.h"

#include "rex_engine/diagnostics/logging.h"
#include "rex_engine/diagnostics/win/win_call.h"

#include <comdef.h>

namespace rex
{
    namespace win32
    {
        //-----------------------------------------------------------------
        rsl::point screen_center()
        {
            card32 screen_width = GetSystemMetrics(SM_CXSCREEN);
            card32 screen_height = GetSystemMetrics(SM_CYSCREEN);

            return { static_cast<int16>(screen_width * 0.5f), static_cast<int16>(screen_height * 0.5f) };
        }

        //-----------------------------------------------------------------
        LResult __stdcall default_win_procedure(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam)
        {
            HWND win_hwnd = reinterpret_cast<HWND>(hwnd);
            if (msg == WM_CREATE)
            {
                CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
                SetWindowLongPtr(win_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create_struct->lpCreateParams));
            }
            else
            {
                Window* this_window = reinterpret_cast<Window*>(GetWindowLongPtrW(win_hwnd, GWLP_USERDATA));
                if (this_window)
                {
                    return this_window->on_event(hwnd, msg, wparam, lparam);
                }
            }

            return DefWindowProc(win_hwnd, msg, wparam, lparam);
        }

        //-----------------------------------------------------------------
        Window::Window(const WindowDescription& description)
            : m_width(description.width)
            , m_height(description.height)
            , m_window_class(description.title, default_win_procedure)
        {
            rsl::point window_left_top = screen_center();
            window_left_top.x -= static_cast<int16>(m_width * 0.5f);
            window_left_top.y -= static_cast<int16>(m_height * 0.5f);

            m_hwnd = WIN_CALL(CreateWindow(
                m_window_class.class_name().data(),
                description.title.data(),
                WS_OVERLAPPED,
                window_left_top.x, window_left_top.y,
                m_width, m_height,
                NULL,
                NULL,
                (HINSTANCE)m_window_class.hinstance(),
                this
            ));

            if (!m_hwnd)
            {
                REX_ERROR("Failed to create window");
            }

            show();

        }

        //-----------------------------------------------------------------
        void Window::update()
        {
            MSG message = { 0 };
            while (PeekMessage(&message, NULL, NULL, NULL, PM_REMOVE) > 0)
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }

        //-----------------------------------------------------------------
        void Window::show()
        {
            ShowWindow((HWND)m_hwnd, SW_SHOW);
        }
        //-----------------------------------------------------------------
        void Window::hide()
        {
            CloseWindow((HWND)m_hwnd);
        }
        //-----------------------------------------------------------------
        void Window::focus()
        {
            WIN_CALL(SetFocus((HWND)m_hwnd));
        }
        //-----------------------------------------------------------------
        void Window::close()
        {
            DestroyWindow((HWND)m_hwnd);
        }
        //-----------------------------------------------------------------
        int32 Window::width() const
        {
            return m_width;
        }
        //-----------------------------------------------------------------
        int32 Window::height() const
        {
            return m_height;
        }

        //-----------------------------------------------------------------
        LResult Window::on_event(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam)
        {
            // Sometimes Windows set error states between messages
            // becasue these aren't our fault, we'll ignore those
            // to make sure our messages are successful
            DWORD last_windows_error = GetLastError();
            rex::win::clear_win_errors();
            
            rsl::ScopeGuard reset_win_error_scopeguard([=]() { SetLastError(last_windows_error); });

            switch (msg)
            {
            case WM_CLOSE:
                close();
            case WM_DESTROY: 
                PostQuitMessage(0); 
                event_system::fire_event(event_system::EventType::WindowClose);
                return 0;
            }

            return DefWindowProc((HWND)hwnd, msg, wparam, lparam);
        }
    }
}
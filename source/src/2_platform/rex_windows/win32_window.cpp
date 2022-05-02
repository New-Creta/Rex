#include "rex_windows_pch.h"

#include "win32_window.h"
#include "win32_event_processor.h"
#include "win32_event_handler.h"

#include "display_mode.h"
#include "display.h"

#include "internal/win_window_class.h"

#include <rex_stl/math/point.h>
#include <rex_stl/diagnostics/win/win_call.h> 

#include <comdef.h>

namespace rex
{
    namespace win32
    {
        const int32 g_min_window_width = 16;
        const int32 g_min_window_height = 16;

        const int32 g_max_window_width = 32767;
        const int32 g_max_window_height = 32767;

        //-----------------------------------------------------------------
        rtl::Point screen_center()
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
                EventHandler* this_event_handler = reinterpret_cast<EventHandler*>(GetWindowLongPtrW(win_hwnd, GWLP_USERDATA));
                if (this_event_handler)
                {
                    return this_event_handler->on_event(hwnd, msg, wparam, lparam);
                }
            }

            return DefWindowProc(win_hwnd, msg, wparam, lparam);
        }

        //-----------------------------------------------------------------
        Window::Window(const WindowDescription& description)
            : m_visible(false)
            , m_width(description.width != 0 ? description.width : 1280)
            , m_height(description.height != 0 ? description.height : 720)
            , m_window_class(description.title, default_win_procedure)
        {
            // R_ASSERT(m_width > g_min_window_width && m_width < g_max_window_width);
            // R_ASSERT(m_height > g_min_window_height && m_height < g_max_window_height);

            m_event_processor = rtl::make_unique<EventProcessor>(this, description.event_callback);
            m_event_processor->enable_event_processing();

            WindowCallbackFunctions callback_functions;

            callback_functions.window_callback = [&](Hwnd hwnd, card32 msg, WParam wparam, LParam lparam)
            {
                handle_window_events(hwnd, msg, wparam, lparam);
            };

            m_event_handler = rtl::make_unique<EventHandler>(*m_event_processor, callback_functions);

            rtl::Point window_left_top = screen_center();
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
                m_event_handler.get()
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
            m_event_handler->peek_events();
        }

        //-----------------------------------------------------------------
        void Window::show()
        {
            if (is_visible())
            {
                return;
            }

            ShowWindow((HWND)m_hwnd, SW_SHOW);

            m_visible = true;
        }
        //-----------------------------------------------------------------
        void Window::hide()
        {
            if (!is_visible())
            {
                return;
            }

            CloseWindow((HWND)m_hwnd);

            m_visible = false;
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

            m_visible = false;
        }

        //-------------------------------------------------------------------------
        bool Window::is_visible() const
        {
            return m_visible;
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

        //-------------------------------------------------------------------------
        int32 Window::get_id() const
        {
            return GetDlgCtrlID((HWND)m_hwnd);
        }

        //-------------------------------------------------------------------------
        void Window::handle_window_events(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam)
        {
            switch (msg)
            {
                case WM_SIZE:
                {
                    m_width = LOWORD(lparam);
                    m_height = HIWORD(lparam);
                }

                break;
            }
        }

    }
}
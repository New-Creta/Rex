#include "rex_windows_pch.h"

#include "win32_event_handler.h"
#include "win32_event_processor.h"

#include "event_queue.h"

#include <rex_stl/diagnostics/win/win_call.h> 
#include <rex_stl/utilities/scopeguard.h>

#include <Windows.h>

namespace rex
{
    namespace win32
    {
        //-------------------------------------------------------------------------
        int32 get_window_id(Hwnd hwnd)
        {
            return GetDlgCtrlID((HWND)hwnd);
        }

        struct EventHandler::Internal
        {
            Internal(EventProcessor& eventProcessor, const WindowCallbackFunctions& callbackFunctions)
                : event_processor(eventProcessor)
                , event_callbacks(callbackFunctions)
            {
            }

            //-------------------------------------------------------------------------
            template <typename T>
            void invoke_callback(const T& callback, Hwnd hwnd, card32 msg, WParam wparam, LParam lparam)
            {
                if (callback)
                {
                    callback(hwnd, msg, wparam, lparam);
                }
            }

            //-------------------------------------------------------------------------
            void peek_events()
            {
                MSG message = { 0 };

                while (PeekMessage(&message, NULL, NULL, NULL, PM_REMOVE) > 0)
                {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
            }
            //-------------------------------------------------------------------------
            LResult on_event(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam)
            {
                // Sometimes Windows set error states between messages
                // becasue these aren't our fault, we'll ignore those
                // to make sure our messages are successful
                DWORD last_windows_error = GetLastError();
                rtl::win::clear_win_errors();

                rtl::ScopeGuard reset_win_error_scopeguard([=]() { SetLastError(last_windows_error); });

                switch (msg)
                {
                case WM_SHOWWINDOW:
                    if (wparam)
                    {
                        invoke_callback(event_callbacks.window_callback, hwnd, msg, wparam, lparam);
                        event_processor.process_window_shown(get_window_id(hwnd));
                    }
                    else
                    {
                        invoke_callback(event_callbacks.window_callback, hwnd, msg, wparam, lparam);
                        event_processor.process_window_hide(get_window_id(hwnd));
                    }
                    break;
                case WM_CLOSE:
                    invoke_callback(event_callbacks.window_callback, hwnd, msg, wparam, lparam);
                    event_processor.process_window_close(get_window_id(hwnd));
                    break;
                case WM_DESTROY:
                    PostQuitMessage(0);
                    return 0;
                }

                return DefWindowProc((HWND)hwnd, msg, wparam, lparam);
            }

            EventProcessor& event_processor;
            WindowCallbackFunctions event_callbacks;
        };

        //-------------------------------------------------------------------------
        EventHandler::EventHandler(EventProcessor& eventProcessor, const WindowCallbackFunctions& callbackFunctions)
            : m_internal(rex::make_internal_scope_ptr<Internal>(eventProcessor, callbackFunctions))
        {
        }
        //-------------------------------------------------------------------------
        EventHandler::~EventHandler()
        {
        }

        //-------------------------------------------------------------------------
        LResult EventHandler::on_event(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam)
        {
            return m_internal->on_event(hwnd, msg, wparam, lparam);
        }

        //-------------------------------------------------------------------------
        void EventHandler::peek_events()
        {
            m_internal->peek_events();
        }
    }
}
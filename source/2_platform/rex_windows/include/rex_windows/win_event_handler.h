#pragma once

#include "rex_windows/win_types.h"

namespace rex
{
    class IWindow;
    namespace win32
    {
        class EventHandler
        {
        public:
            explicit EventHandler(IWindow* wnd);

            LResult on_event(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam);

        private:
            IWindow* m_wnd;
        };
    } // namespace win32
} // namespace rex
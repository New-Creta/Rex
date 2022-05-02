#pragma once

#include "win_types.h"
#include "memory/internal_ptr.h"

namespace rex
{
    namespace win32
    {
        class EventProcessor;

        struct WindowCallbackFunctions
        {
            using WindowCallbackFn = std::function<void(Hwnd, card32, WParam, LParam)>;

            WindowCallbackFn window_callback;
        };

        class EventHandler
        {
        public:
            EventHandler(EventProcessor& eventProcessor, const WindowCallbackFunctions& callbackFunctions);
            ~EventHandler();

            LResult on_event(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam);
            void peek_events();

        private:
            struct Internal;
            rex::internal_scope_ptr<Internal> m_internal;
        };
    }
}
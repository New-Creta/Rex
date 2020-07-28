#pragma once

#include "events/win32messageparameters.h"

namespace rex
{
    class Window;

    namespace events
    {
        class Event;
    }

    namespace win32
    {
        class EventProcessor
        {
        public:
            using EventCallbackFn = std::function<void(events::Event&)>;

            EventProcessor(rex::Window* window, EventCallbackFn callback);
            ~EventProcessor();

            void enableEventProcessing();
            void disableEventProcessing();

            LRESULT process(HWND hwnd, UINT msg, rex::win32::MessageParameters parameters);

        private:
            Window* m_window;

            EventCallbackFn m_callback;

            bool m_enabled;
        };
    }
}
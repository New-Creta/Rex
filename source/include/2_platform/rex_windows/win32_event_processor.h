#pragma once

#include "event.h"

struct SDL_Window;
struct SDL_WindowEvent;
struct SDL_KeyboardEvent;
struct SDL_TextInputEvent;
struct SDL_MouseButtonEvent;
struct SDL_MouseWheelEvent;
struct SDL_MouseMotionEvent;

namespace rex
{
    namespace win32
    {
        class Window;

        class EventProcessor
        {
        public:
            EventProcessor(Window* window, events::EventCallbackFn callback);
            ~EventProcessor();

            void enable_event_processing();
            void disable_event_processing();

            void process_window_shown(int32 windowID/*, const memory::Blob& nativeEvt*/);
            void process_window_hide(int32 windowID/*, const memory::Blob& nativeEvt*/);
            void process_window_close(int32 windowID/*, const memory::Blob& nativeEvt*/);

        private:
            bool m_enabled;
            events::EventCallbackFn m_callback;
            Window* m_window;
        };
    }
}
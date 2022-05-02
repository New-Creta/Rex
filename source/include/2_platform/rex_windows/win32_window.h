#pragma once

#include "core_window.h"
#include "win_types.h"
#include "internal/win_window_class.h"

namespace rex
{
    class Display;

    namespace win32
    {
        class EventProcessor;
        class EventHandler;

        class Window : public CoreWindow
        {
        public:
            Window(const WindowDescription& description);

            void update() override;

            void show() override;
            void hide() override;
            void focus() override;
            void close() override;

            bool is_visible() const override;

            int32 width() const override;
            int32 height() const override;

            int32 get_id() const override;

        private:
            void handle_window_events(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam);

            bool m_visible;

            int32 m_width;
            int32 m_height;

            WindowClass m_window_class;

            Hwnd m_hwnd;

            rtl::UniquePtr<EventProcessor> m_event_processor;
            rtl::UniquePtr<EventHandler> m_event_handler;
        };
    }
}
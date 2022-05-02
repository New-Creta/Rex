#pragma once

#include "core_window.h"
#include "win_types.h"
#include "internal/win_window_class.h"

#include <rex_stl/memory/unique_ptr.h>

struct GLFWwindow;

namespace rex
{
    namespace win32
    {
        class Window : public CoreWindow
        {
        public:
            Window(const WindowDescription& description);

            void update() override;

            void show() override;
            void hide() override;
            
            void focus() override;
            void close() override;

            int32 width() const override;
            int32 height() const override;

            LResult on_event(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam);

        private:
            int32 m_width;
            int32 m_height;

            WindowClass m_window_class;

            Hwnd m_hwnd;
        };
    }
}
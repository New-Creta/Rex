#pragma once

#include "core_window.h"

struct GLFWwindow;

namespace rex
{
    namespace win32
    {
        class Window : public CoreWindow
        {
        public:
            Window(const WindowDescription& description);
            ~Window() override;

            void update() override;

            void show() override;
            void hide() override;

            void make_windowed(int32 newWidth = -1, int32 newHeight = -1) override;
            void make_fullscreen(int32 newWidth = -1, int32 newHeight = -1) override;

            int32 get_width() const override;
            int32 get_height() const override;

        private:
            int32 m_width;
            int32 m_height;

            bool m_fullscreen;

            GLFWwindow* m_glfw_window;
        };
    }
}
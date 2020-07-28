#pragma once

#include "rex_platform_global.h"

#include "core/window.h"

#include "events/win32eventprocessor.h"

namespace rex
{
    namespace win32
    {
        class Window : public rex::Window
        {
        public:
            REX_PLATFORM_EXPORT Window(const WindowProperties& properties);
            REX_PLATFORM_EXPORT ~Window() override;
                
            REX_PLATFORM_EXPORT void show() override;
            REX_PLATFORM_EXPORT void hide() override;
                
            REX_PLATFORM_EXPORT void update() override;
                
            REX_PLATFORM_EXPORT unsigned int getWidth() const override;
            REX_PLATFORM_EXPORT unsigned int getHeight() const override;
                
            REX_PLATFORM_EXPORT void* getHandle() override;

        private:
            static LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);

            void setupWindowClass();
            void setupHwnd();
            void setupPixelFormat();

            bool                m_visible;

            HINSTANCE           m_hinstance;
            HWND                m_hwnd;
            WNDPROC             m_wndproc;

            EventProcessor      m_event_processor;

            int                 m_width;
            int                 m_height;

            std::string         m_title;
        };
    }
}
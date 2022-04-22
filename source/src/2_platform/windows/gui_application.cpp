#include "windows_pch.h"

#include "gui_application.h"
#include "window.h"

#include "event_system.h"

#include <rex_stl/memory/unique_ptr.h>

namespace rex
{
    namespace win32
    {
        struct GuiApplication::Internal
        {
            rtl::UniquePtr<CoreWindow> window;
        };

        //-------------------------------------------------------------------------
        GuiApplication::GuiApplication(const ApplicationDescription& appDescription)
            :CoreApplication(appDescription)
            ,m_internal_ptr(std::make_unique<Internal>())
        {}
        //-------------------------------------------------------------------------
        GuiApplication::~GuiApplication() = default;

        //-------------------------------------------------------------------------
        void GuiApplication::platform_initialize()
        {
            m_internal_ptr->window.reset(create_window());

            event_system::subscribe(event_system::EventType::WindowClose, [this]() { mark_for_destroy(); });

            app_initialize();
        }
        //-------------------------------------------------------------------------
        void GuiApplication::platform_update(const FrameInfo& info)
        {
            m_internal_ptr->window->update();

            app_update(info);
        }
        //-------------------------------------------------------------------------
        void GuiApplication::platform_shutdown()
        {
            app_shutdown();
        }
    }
}
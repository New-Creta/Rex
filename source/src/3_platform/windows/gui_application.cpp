#include "windows_pch.h"

#include "gui_application.h"

namespace rex
{
    namespace win32
    {
        //-------------------------------------------------------------------------
        GuiApplication::GuiApplication(const ApplicationDescription& description)
            :CoreApplication(description)
        {}
        //-------------------------------------------------------------------------
        GuiApplication::~GuiApplication() = default;

        //-------------------------------------------------------------------------
        void GuiApplication::platform_initialize()
        {
            app_initialize();
        }
        //-------------------------------------------------------------------------
        void GuiApplication::platform_update(const FrameInfo& info)
        {
            app_update(info);
        }
        //-------------------------------------------------------------------------
        void GuiApplication::platform_shutdown()
        {
            app_shutdown();
        }
    }
}
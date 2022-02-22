#include "windows_pch.h"

#include "gui_application.h"
#include "window.h"

#include <GLFW/glfw3.h>

namespace rex
{
    namespace win32
    {
        struct GuiApplication::Internal
        {
            bool init_glfw()
            {
                int32 glfw_result = glfwInit();

                REX_ASSERT(glfw_result == GLFW_TRUE);
                if (!glfw_result)
                {
                    std::cout << "GLFW failed to init" << std::endl;
                    return false;
                }

                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

                return true;
            }

            std::unique_ptr<CoreWindow> m_window;
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
            if (!m_internal_ptr->init_glfw())
            {
                return;
            }

            m_internal_ptr->m_window = std::unique_ptr<CoreWindow>(create_window());

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

            glfwTerminate();
        }
    }
}
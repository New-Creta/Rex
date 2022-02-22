#include "windows_pch.h"

#include "window.h"

#include <GLFW/glfw3.h>

namespace rex
{
    namespace win32
    {
        //-----------------------------------------------------------------
        Window::Window(const WindowDescription& description)
            :m_width(description.width)
            ,m_height(description.height)
            ,m_fullscreen(description.fullscreen)
        {
            m_glfw_window = glfwCreateWindow(description.width, description.height, description.title.c_str(), description.fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

            REX_ASSERT(m_glfw_window != nullptr);
            if (m_glfw_window == nullptr)
            {
                std::cout << "Failed to create GLFW window" << std::endl;
            }
        }
        //-----------------------------------------------------------------
        Window::~Window()
        {
            glfwDestroyWindow(m_glfw_window);
        }

        //-----------------------------------------------------------------
        void Window::update()
        {
            while (!glfwWindowShouldClose(m_glfw_window))
            {
                glfwSwapBuffers(m_glfw_window);
                glfwPollEvents();
            }
        }

        //-----------------------------------------------------------------
        void Window::show()
        {
            glfwShowWindow(m_glfw_window);
        }
        //-----------------------------------------------------------------
        void Window::hide()
        {
            glfwHideWindow(m_glfw_window);
        }
        //-----------------------------------------------------------------
        void Window::make_windowed(int32 newWidth /*= -1*/, int32 newHeight /*= -1*/)
        {
            m_width = newWidth != -1 && newWidth != m_width ? newWidth : m_width;
            m_height = newHeight != -1 && newHeight != m_height ? newHeight : m_height;

            const GLFWvidmode* desktop = glfwGetVideoMode(glfwGetPrimaryMonitor());

            int32 posx = 0;
            int32 posy = 0;
            if (desktop)
            {
                posx = static_cast<int32>(std::round((desktop->width - m_width) * 0.5f));
                posy = static_cast<int32>(std::round((desktop->height - m_height) * 0.5f));
            }

            const int32 refresh_rate = GLFW_DONT_CARE;
            glfwSetWindowMonitor(m_glfw_window, NULL, posx, posy, m_width, m_height, refresh_rate);

            m_fullscreen = false;
        }
        //-----------------------------------------------------------------
        void Window::make_fullscreen(int32 newWidth /*= -1*/, int32 newHeight /*= -1*/)
        {
            m_width = newWidth != -1 && newWidth != m_width ? newWidth : m_width;
            m_height = newHeight != -1 && newHeight != m_height ? newHeight : m_height;

            const int32 refresh_rate = GLFW_DONT_CARE;
            glfwSetWindowMonitor(m_glfw_window, glfwGetPrimaryMonitor(), 0, 0, m_width, m_height, refresh_rate);

            m_fullscreen = true;
        }
        //-----------------------------------------------------------------
        int32 Window::get_width() const
        {
            return m_width;
        }
        //-----------------------------------------------------------------
        int32 Window::get_height() const
        {
            return m_height;
        }
    }
}
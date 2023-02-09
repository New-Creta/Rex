#include "rex_engine_opengl_pch.h"

///////////////////////////////////////////////
// These includes have to be defined first
// Otherwise multiple entries of " #include "gl.h" " are introduced
// " #pragma once " does not solve this
//
#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
//
///////////////////////////////////////////////

#include "gl_context.h"

namespace rex
{
    namespace opengl
    {
        //-------------------------------------------------------------------------
        OpenGLContext::OpenGLContext(const WindowHandle& hwnd)
            :IGraphicsContext()
            ,m_handle(hwnd)
        {
        }

        //-------------------------------------------------------------------------
        void OpenGLContext::resize(int32 x, int32 y, int32 width, int32 height)
        {
            glViewport(x, y, width, height);
        }
        //-------------------------------------------------------------------------
        void OpenGLContext::make_current()
        {
            glfwMakeContextCurrent(m_handle.as_glfw_ptr);
        }
        //-------------------------------------------------------------------------
        void OpenGLContext::present()
        {
            glfwSwapBuffers(m_handle.as_glfw_ptr);
        }
    }
}
#include "rex_platform_pch.h"
#include "rex_platform_defines.h"

#include "api/opengl/win32glcontext.h"

#ifdef _OPENGL

namespace
{
    //-------------------------------------------------------------------------
    PIXELFORMATDESCRIPTOR createWindowPixelFormatDescription()
    {
        PIXELFORMATDESCRIPTOR result = {};
        result.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        result.nVersion = 1;
        result.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        result.iPixelType = PFD_TYPE_RGBA;
        result.cColorBits = 32;
        result.cDepthBits = 24;
        result.cStencilBits = 8;
        result.cAuxBuffers = 0;
        result.iLayerType = PFD_MAIN_PLANE;
        return result;
    }

    //-------------------------------------------------------------------------
    void setupPixelFormat(HDC hdc)
    {
        PIXELFORMATDESCRIPTOR pfd = createWindowPixelFormatDescription();

        auto choose_pixel_format = ChoosePixelFormat(hdc, &pfd);
        RX_ASSERT_X(choose_pixel_format != NULL, "Pixel format is null");

        auto set_pixel_format = SetPixelFormat(hdc, choose_pixel_format, &pfd);
        RX_ASSERT_X(set_pixel_format == TRUE, "Failed to set pixel format");
    }
}

//-------------------------------------------------------------------------
std::unique_ptr<rex::win32::Context> rex::win32::Context::create(void* handle)
{
    HWND hwnd = static_cast<HWND>(handle);
    HDC hdc = GetDC(hwnd);

    setupPixelFormat(hdc);

    std::unique_ptr<Context> context = std::make_unique<Context>(hwnd);
    if (!context->create())
    {
        return nullptr;
    }

    ReleaseDC(hwnd, hdc);

    return context;
}
//-------------------------------------------------------------------------
void rex::win32::Context::destroy(Context* context)
{
    context->destroy();
}
//-------------------------------------------------------------------------
void rex::win32::Context::makeCurrent(Context* context)
{
    context->setAsCurrent();
}

//-------------------------------------------------------------------------
rex::win32::Context::Context(HWND hwnd)
    :m_hwnd(hwnd)
    ,m_hdc(GetDC(hwnd))
    ,m_context(NULL)
{
    RX_INFO("API target: {}", REX_API_TARGET);
}
//-------------------------------------------------------------------------
rex::win32::Context::~Context()
{
    ReleaseDC(m_hwnd, m_hdc);
}

//-------------------------------------------------------------------------
bool rex::win32::Context::create()
{
    /* Create the GL 3.x context */
    m_context = wglCreateContext(m_hdc);
    if (m_context == NULL)
    {
        RX_ERROR("OpenGL 3.x was not created!");
        return false;
    }

    return true;
}
//-------------------------------------------------------------------------
bool rex::win32::Context::destroy()
{
    if (wglDeleteContext(m_context) == false)
    {
        RX_ERROR("Could not destroy the given context");
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------
void rex::win32::Context::setAsCurrent()
{
    if (wglMakeCurrent(m_hdc, m_context) == false)
    {
        RX_ERROR("Could not make the given context the current one");
        return;
    }
}

#endif
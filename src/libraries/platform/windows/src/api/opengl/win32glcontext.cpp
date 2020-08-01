#include "rex_platform_pch.h"

#include "api/opengl/win32glcontext.h"

#ifdef _OPENGL

//-------------------------------------------------------------------------
std::unique_ptr<rex::win32::Context> rex::win32::Context::create(void* handle)
{
    HWND hwnd = static_cast<HWND>(handle);

    std::unique_ptr<Context> context = std::make_unique<Context>(hwnd);
    if (!context->create())
    {
        return nullptr;
    }

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
{}
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
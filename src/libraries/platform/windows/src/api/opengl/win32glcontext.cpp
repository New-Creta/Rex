#include "rex_platform_pch.h"
#include "api/opengl/win32glcontext.h"

#ifdef _OPENGL

#ifndef WGL_ARB_create_context
#define WGL_ARB_create_context
#define WGL_CONTEXT_MAJOR_VERSION_ARB                   0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB                   0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB                     0x2093
#define WGL_CONTEXT_FLAGS_ARB                           0x2094
#define WGL_CONTEXT_DEBUG_BIT_ARB                       0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB          0x0002

#ifndef WGL_ARB_create_context_profile
#define WGL_ARB_create_context_profile
#define WGL_CONTEXT_PROFILE_MASK_ARB                    0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB                0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB       0x00000002
#endif

#ifndef WGL_ARB_create_context_robustness
#define WGL_ARB_create_context_robustness
#define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB               0x00000004
#define WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB     0x8256
#define WGL_NO_RESET_NOTIFICATION_ARB                   0x8261
#define WGL_LOSE_CONTEXT_ON_RESET_ARB                   0x8252
#endif
#endif

#ifndef WGL_ARB_context_flush_control
#define WGL_ARB_context_flush_control
#define WGL_CONTEXT_RELEASE_BEHAVIOR_ARB                0x2097
#define WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB           0x0000
#define WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB          0x2098
#endif

#ifndef WGL_ARB_create_context_no_error
#define WGL_ARB_create_context_no_error
#define WGL_CONTEXT_OPENGL_NO_ERROR_ARB                 0x31B3
#endif

namespace
{
    //-------------------------------------------------------------------------
    void setOpenGLVersion(int* attribs, int& attribCount)
    {
        int major;
        int minor;

        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        attribs[attribCount++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
        attribs[attribCount++] = major;
        attribs[attribCount++] = WGL_CONTEXT_MINOR_VERSION_ARB;
        attribs[attribCount++] = minor;
    }

    //-------------------------------------------------------------------------
    void setProfileMask(int* attribs, int& attribCount, int profileMask)
    {
        attribs[attribCount++] = WGL_CONTEXT_PROFILE_MASK_ARB;
        attribs[attribCount++] = profileMask;
    }
    //-------------------------------------------------------------------------
    void setFlags(int* attribs, int& attribCount, int flags)
    {
        attribs[attribCount++] = WGL_CONTEXT_FLAGS_ARB;
        attribs[attribCount++] = flags;
    }

    //-------------------------------------------------------------------------
    void setContextRobustness(int* attribs, int& attribCount, bool resetNotification)
    {
#ifndef WGL_ARB_create_context
        attribs[attribCount++] = WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB;
        attribs[attribCount++] = resetNotification
            ? WGL_LOSE_CONTEXT_ON_RESET_ARB
            : WGL_NO_RESET_NOTIFICATION_ARB;
#else
        UNUSED_PARAM(attribs);
        UNUSED_PARAM(attribCount);
        UNUSED_PARAM(resetNotification);
#endif
    }
    //-------------------------------------------------------------------------
    void setContextFlushControl(int* attribs, int& attribCount, bool releaseBehavior)
    {
#ifndef WGL_ARB_context_flush_control
        attribs[attribCount++] = WGL_CONTEXT_RELEASE_BEHAVIOR_ARB;
        attribs[attribCount++] = releaseBehavior
            ? WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB
            : WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB;
#else
        UNUSED_PARAM(attribs);
        UNUSED_PARAM(attribCount);
        UNUSED_PARAM(releaseBehavior);
#endif
    }
    //-------------------------------------------------------------------------
    void setContextNoError(int* attribs, int& attribCount, bool noError)
    {
#ifndef WGL_ARB_create_context_no_error
        attribs[attribCount++] = WGL_CONTEXT_OPENGL_NO_ERROR_ARB;
        attribs[attribCount++] = noError;
#else
        UNUSED_PARAM(attribs);
        UNUSED_PARAM(attribCount);
        UNUSED_PARAM(noError);
#endif
    }
}

//-------------------------------------------------------------------------
rex::win32::Context::Context(const rex::gl::DriverData& data)
    :rex::gl::Context(data)
{}
//-------------------------------------------------------------------------
rex::win32::Context::~Context() = default;

//-------------------------------------------------------------------------
void* rex::win32::Context::createLegacyContext()
{
    HGLRC share_context = glContextConfiguration().share_with_current_context
        ? static_cast<HGLRC>(wglGetCurrentContext())
        : nullptr;

    HWND hwnd = static_cast<HWND>(driverData().device_context);
    HDC hdc = GetDC(hwnd);

    HGLRC context = wglCreateContext(hdc);
    if (share_context != nullptr)
    {
        wglShareLists(share_context, context);
    }

    /* Release acquired DC */
    ReleaseDC(hwnd, hdc);

    return !context
        ? nullptr
        : context;
}
//-------------------------------------------------------------------------
void* rex::win32::Context::createModernContext()
{
    HGLRC context = nullptr;
    HGLRC share_context = glContextConfiguration().share_with_current_context
        ? (HGLRC)wglGetCurrentContext()
        : nullptr;

    HWND hwnd = static_cast<HWND>(driverData().device_context);
    HDC hdc = GetDC(hwnd);

    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

    void* temp_context = initializeTemporaryContext();
    if (!temp_context)
    {
        ReleaseDC(hwnd, hdc);

        return nullptr;
    }

    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    if (!wglCreateContextAttribsARB)
    {
        ReleaseDC(hwnd, hdc);

        RX_WARN("OpenGL 3.x is not supported");
        return temp_context;
    }

    int attribs[15];  /* max 14 attributes plus terminator */
    int iattr = 0;

    setOpenGLVersion(attribs, iattr);

    setProfileMask(attribs, iattr, glContextConfiguration().profile_mask);
    setFlags(attribs, iattr, glContextConfiguration().flags);

    setContextRobustness(attribs, iattr, glContextData().HAS_WGL_ARB_create_context_robustness);
    setContextFlushControl(attribs, iattr, glContextData().HAS_WGL_ARB_context_flush_control);
    setContextNoError(attribs, iattr, glContextData().HAS_WGL_ARB_create_context_no_error);

    attribs[iattr++] = 0;

    /* Create the GL 3.x context */
    context = wglCreateContextAttribsARB(hdc, share_context, attribs);

    /* Delete the GL 2.x context */
    wglDeleteContext(static_cast<HGLRC>(temp_context));

    /* Release acquired DC */
    ReleaseDC(hwnd, hdc);

    return context;
}

//-------------------------------------------------------------------------
void rex::win32::Context::makeCurrent(void* context)
{
    HWND hwnd = static_cast<HWND>(driverData().device_context);
    HDC hdc = GetDC(hwnd);

    if (wglMakeCurrent(hdc, static_cast<HGLRC>(context)) == false)
    {
        ReleaseDC(hwnd, hdc);
        RX_ERROR("Could not make the given context the current one");
        return;
    }

    ReleaseDC(hwnd, hdc);
}
//-------------------------------------------------------------------------
void rex::win32::Context::deleteContext(void* context)
{
    if (wglDeleteContext(static_cast<HGLRC>(context)) == false)
    {
        RX_ERROR("Could not destroy the given context");
        return;
    }
}

//-------------------------------------------------------------------------
void* rex::win32::Context::initializeTemporaryContext()
{
    void* temp_context = createLegacyContext();
    if (!temp_context)
    {
        return nullptr;
    }

    makeCurrent(temp_context);

    return temp_context;
}

#endif
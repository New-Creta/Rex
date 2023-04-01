#include "rex_windows/log.h"
#include "rex_engine/types.h"
#include "rex_renderer_core/context.h"

#include <Windows.h>
#include <glad/gl.h>
#include <glad/wgl.h>

// NOLINTBEGIN(modernize-use-nullptr,-warnings-as-errors)

namespace rex
{
  struct WglContext
  {
    HDC dc;
    HGLRC glrc;
  };

  WglContext g_glctx; // NOLINT (fuchsia-statically-constructed-objects,-warnings-as-errors, cppcoreguidelines-avoid-non-const-global-variables,-warnings-as-errors)

  bool rex_make_gl_context_current()
  {
    return static_cast<bool>(wglMakeCurrent(g_glctx.dc, g_glctx.glrc));
  }
  void rex_gl_swap_buffers()
  {
    SwapBuffers(g_glctx.dc);
  }

  namespace context
  {
    namespace os
    {
      void release_device_context(HWND hwnd, HDC dc)
      {
        // The application must call the ReleaseDC function for each call to the GetWindowDC function
        // and for each call to the GetDC function that retrieves a common DC.
        // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-releasedc
        ReleaseDC(hwnd, dc);
      }

      //-------------------------------------------------------------------------
      bool pre_setup(void* userData)
      {
        HWND hwnd = static_cast<HWND>(userData);

        g_glctx.dc = GetDC(hwnd);

        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd, 0, sizeof(pfd));

        pfd.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion     = 1;
        pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType   = PFD_TYPE_RGBA;
        pfd.cColorBits   = 32;
        pfd.cAlphaBits   = 8;
        pfd.cDepthBits   = 24;
        pfd.cStencilBits = 8;
        pfd.iLayerType   = PFD_MAIN_PLANE;

        const s32 pf = ChoosePixelFormat(g_glctx.dc, &pfd);
        if(pf == NULL || SetPixelFormat(g_glctx.dc, pf, &pfd) == FALSE)
        {
          release_device_context(hwnd, g_glctx.dc);
          REX_ERROR(LogWindows, "Failed to set a compatible pixel format");
          return false;
        }

        HGLRC temp = wglCreateContext(g_glctx.dc);
        if(temp == NULL)
        {
          release_device_context(hwnd, g_glctx.dc);
          REX_ERROR(LogWindows, "Failed to create the initial rendering context");
          return false;
        }

        wglMakeCurrent(g_glctx.dc, temp);

        gladLoaderLoadWGL(g_glctx.dc);

        rsl::array attribs = {WGL_CONTEXT_MAJOR_VERSION_ARB,
                              3, // Set the MAJOR version of OpenGL to 3
                              WGL_CONTEXT_MINOR_VERSION_ARB,
                              2, // Set the MINOR version of OpenGL to 2
                              WGL_CONTEXT_FLAGS_ARB,
                              WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, // Set our OpenGL context to be forward compatible
                              0};

        const char* extensions = wglGetExtensionsStringARB(g_glctx.dc);
        if(rsl::strstr(extensions, "WGL_ARB_create_context") == NULL)
        {
          g_glctx.glrc = wglCreateContextAttribsARB(g_glctx.dc, NULL, attribs.data());
          if(g_glctx.glrc == NULL)
          {
            wglDeleteContext(temp);
            release_device_context(hwnd, g_glctx.dc);
            REX_ERROR(LogWindows, "Failed to create Windows OpenGL Context");

            return false;
          }

          wglMakeCurrent(NULL, NULL);
          wglDeleteContext(temp);
          wglMakeCurrent(g_glctx.dc, g_glctx.glrc);
        }
        else
        {
          g_glctx.glrc = temp;
        }

        return true;
      }

      //-------------------------------------------------------------------------
      bool post_setup(void* /* userData */)
      {
        // Nothing to implement

        return true;
      }

      //-------------------------------------------------------------------------
      void rollback(void* userData)
      {
        wglMakeCurrent(NULL, NULL);

        if(g_glctx.glrc != NULL)
        {
          wglDeleteContext(g_glctx.glrc);
        }

        if(g_glctx.dc != NULL)
        {
          HWND hwnd = static_cast<HWND>(userData);
          release_device_context(hwnd, g_glctx.dc);
        }
      }
    } // namespace os
  }   // namespace context
} // namespace rex

// NOLINTEND(modernize-use-nullptr,-warnings-as-errors)
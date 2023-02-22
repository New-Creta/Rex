#include "rex_windows/win_window.h"

#include "rex_engine/diagnostics/logging.h"

#include <GL/glx.h>
#include <GL/glxext.h>
#include <X11/Xlib.h>

namespace rex
{
    Display* _display;
    Window _window;

    bool rex_make_gl_context_current()
    {
        //return static_cast<bool>(wglMakeCurrent(g_glctx.dc, g_glctx.glrc));
        return true;
    }

    void rex_gl_swap_buffers()
    {
        //SwapBuffers(g_glctx.dc);
    }

  namespace linux
  {
    XIM _xim;
    XIC _xic;

    bool _invalidate_window_frame = false;

    //-------------------------------------------------------------------------
    Window::Window()
        :m_destroyed(false)
    {
    }

    //-------------------------------------------------------------------------
    bool Window::create(const WindowDescription& description)
    {       
        _display = XOpenDisplay(NULL);

        Visual* visual = nullptr;
        s32 depth = 0;
        s32 screen = 0;

        {
            constexpr s32 screen_monitor = 0;

            visual = DefaultVisual(_display, screen_monitor);
            depth = DefaultDepth(_display, screen_monitor);
            screen = DefaultScreen(_display);
        }

        XSetWindowAttributes swa;
        
        //swa.colormap = XCreateColormap(_display, RootWindow(_display, vi->screen), vi->visual, AllocNone);
        swa.colormap = XCreateColormap(_display, XRootWindow(_display, screen), visual, AllocNone);
        swa.background_pixmap = None;
        swa.border_pixel = 0;
        swa.event_mask = StructureNotifyMask;

        _window = XCreateWindow(_display,
                                // RootWindow(_display, vi->screen), 
                                XRootWindow(display, screen),
                                0, 
                                0, 
                                description.viewport.width, 
                                description.viewport.height, 
                                0,
                                //vi->depth, 
                                depth,
                                InputOutput, 
                                //vi->visual, 
                                visual,
                                CWBorderPixel | CWColormap | CWEventMask, 
                                &swa);

        XStoreName(_display, _window, description.title);
        XSelectInput(_display, _window, ExposureMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | ConfigureNotify);

        XMapWindow(_display, _window);

        _xim = XOpenIM(_display, NULL, NULL, NULL);
        if (_xim == NULL)
        {
            REX_ERROR("Could not open input method\n");
            return false;
        }

        _xic = XCreateIC(_xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, _window, NULL);
        if (_xic == NULL)
        {
            REX_ERROR("Could not open IC\n");
            return false;
        }

        XSetICFocus(_xic);

        return true;
    }

    //-------------------------------------------------------------------------
    void Window::update()
    {
        while (XPending(_display) > 0)
        {
            XEvent event;
            XNextEvent(_display, (XEvent*)&event);
            switch (event.type)
            {
                case Expose:
                case ConfigureNotify:
                {
                    XWindowAttributes attribs;
                    XGetWindowAttributes(_display, _window, &attribs);
                    m_frame.width = attribs.width;
                    m_frame.height = attribs.height;

                    Window unused;
                    XTranslateCoordinates(_display, _window, XDefaultRootWindow(_display), 0, 0, (s32*)&m_frame.x, (s32*)&m_frame.y, &unused);
                }
                break;
            }
        }

        if (_invalidate_window_frame)
        {
            XMoveResizeWindow(_display, _window, m_frame.x, m_frame.y, m_frame.width, m_frame.height);
            _invalidate_window_frame = false;
        }
    }
    //-----------------------------------------------------------------
    void Window::show()
    {
        XRaiseWindow(_display, _window);
    }
    //-----------------------------------------------------------------
    void Window::hide()
    {
        XLowerWindow(_display, _window);
    }
    //-----------------------------------------------------------------
    void Window::close()
    {
      destroy();
    }

    //-------------------------------------------------------------------------
    void* Window::primary_display_handle()
    {
      return reinterpret_cast<void*>(&_window);
    }

    //-------------------------------------------------------------------------
    s32 Window::width() const
    {
        return m_frame.width;
    }
    //-------------------------------------------------------------------------
    s32 Window::height() const
    {
        return m_frame.height;
    }

    //-------------------------------------------------------------------------
    f32 Window::aspect() const
    {
      const s32 w = width();
      const s32 h = height();

      return static_cast<f32>(w) / static_cast<f32>(h);
    }

    //-------------------------------------------------------------------------
    bool Window::destroy()
    {
      if(m_destroyed == false) // NOLINT(readability-simplify-boolean-expr)
      {
        XDestroyWindow(_display, _window);
        XCloseDisplay(_display);
      }
      else
      {
        REX_WARN("Window already destroyed");
        return false;
      }

      m_destroyed = true;
      return true;
    }
  } // namespace win32
} // namespace rex
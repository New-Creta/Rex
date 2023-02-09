#include "rex_windows/gui_application.h"

#include "rex_engine/diagnostics/logging.h"
#include "rex_engine/event_system.h"
#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/frameinfo/fps.h"
#include "rex_engine/frameinfo/frameinfo.h"

#include "rex_std/bonus/utility/scopeguard.h"
#include "rex_std/math.h"
#include "rex_std/memory.h"
#include "rex_std/thread.h"

#include "rex_windows/platform_creation_params.h"
#include "rex_windows/win_window.h"

#include "rex_renderer_core/renderer.h"

#include <Windows.h>

#if REX_RENDERER_OPENGL
    #define GLEW_STATIC

    #include "GL/glew.h"
    #include "GL/wglew.h"
    
    struct wgl_context
    {
        HDC   dc;
        HGLRC glrc;
    };

    wgl_context s_glctx;

    namespace rex
    {
      extern bool rex_make_gl_context_current()
      {
          return wglMakeCurrent(s_glctx.dc, s_glctx.glrc);
      }
      extern void rex_gl_swap_buffers()
      {
          SwapBuffers(s_glctx.dc);
      }

      namespace opengl
      {
          bool create_gl_context(HWND hwnd)
          {
              s_glctx.dc = GetDC(hwnd);

              PIXELFORMATDESCRIPTOR pfd;
              memset(&pfd, 0, sizeof(pfd));

              pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
              pfd.nVersion = 1;
              pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
              pfd.iPixelType = PFD_TYPE_RGBA;
              pfd.cColorBits = 32;
              pfd.cAlphaBits = 8;
              pfd.cDepthBits = 24;
              pfd.cStencilBits = 8;
              pfd.iLayerType = PFD_MAIN_PLANE;

              int pf = ChoosePixelFormat(s_glctx.dc, &pfd);
              DescribePixelFormat(s_glctx.dc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
              int   result = SetPixelFormat(s_glctx.dc, pf, &pfd);
              HGLRC temp = wglCreateContext(s_glctx.dc);
              wglMakeCurrent(s_glctx.dc, temp);

              int attribs[] = { WGL_CONTEXT_MAJOR_VERSION_ARB, 4, WGL_CONTEXT_MINOR_VERSION_ARB, 3, WGL_CONTEXT_FLAGS_ARB, 0, 0 };

              // Create OpenGL context
              int error = glewInit();
              if (wglewIsSupported("WGL_ARB_create_context") == 1)
              {
                  s_glctx.glrc = wglCreateContextAttribsARB(s_glctx.dc, 0, attribs);
                  wglMakeCurrent(NULL, NULL);
                  wglDeleteContext(temp);
                  result = wglMakeCurrent(s_glctx.dc, s_glctx.glrc);
              }
              else
              {
                  s_glctx.glrc = temp;
              }

              return true;
          }
      }
    }

    #define create_ctx(hwnd) rex::opengl::create_gl_context(hwnd)
#else
    #define create_ctx(hwnd) (hwnd == nullptr)
#endif

namespace rex
{
  namespace win32
  {
    struct GuiApplication::Internal
    {
      Internal(const PlatformCreationParams& guiCreationParams, const ApplicationCreationParams& appParams, CommandLineArguments&& genericCreationParams)
          : platform_creation_params(rsl::move(guiCreationParams))
          , app_params(appParams)
          , cmd_line_args(rsl::move(genericCreationParams))
      {
      }

      bool initialize()
      {
        window = create_window();
        if(window == nullptr)
        {
          return false;
        }

        HWND hwnd = (HWND)window->get_primary_display_handle();
        if(create_ctx(hwnd) == false)
        {
          return false;
        }
        
        if(renderer::initialize((void*)&hwnd, app_params.max_render_commands) == false)
        {
          return false;
        }

        return on_initialize();
      }
      void update()
      {
        is_running = true;

        while(is_running)
        {
          loop();

          is_running = !is_marked_for_destroy;
        }
      }
      void shutdown()
      {
        on_shutdown();

        if(window)
        {
          window->destroy();
        }
      }

      void loop()
      {
        const FrameInfo info = {m_delta_time, m_fps};
        
        on_update(info);

        m_delta_time.update();
        m_fps.update();

        window->update();

        is_running = !is_marked_for_destroy;

        //
        // Cap framerate to "max_fps".
        // Safe resources of the machine we are running on.
        //
        const rsl::chrono::milliseconds actual_time(static_cast<int64>(rsl::lrint(1000.0f / static_cast<f32>(m_fps.get()))));
        const rsl::chrono::milliseconds desired_time(static_cast<int64>(rsl::lrint(1000.0f / static_cast<f32>(app_params.max_fps))));

        const rsl::chrono::duration<float> elapsed_time = desired_time - actual_time;
        using namespace rsl::chrono_literals; // NOLINT(google-build-using-namespace)
        if(elapsed_time > 0ms)
        {
          rsl::this_thread::sleep_for(elapsed_time);
        }
      }

      rsl::unique_ptr<Window> create_window()
      {
        auto wnd = rsl::make_unique<Window>();

        WindowDescription wnd_description;
        wnd_description.title    = app_params.window_title;
        wnd_description.viewport = {0, 0, app_params.window_width, app_params.window_height};

        if(wnd->create(platform_creation_params.instance, platform_creation_params.show_cmd, wnd_description))
        {
          return wnd;
        }

        REX_ERROR("Window was not created returning nullptr");
        return nullptr;
      }

      void subscribe_window_events()
      {
        event_system::subscribe(event_system::EventType::WindowClose, [this]() { mark_for_destroy(); });
      }

      bool running() const
      {
        return is_running;
      }
      bool marked_for_destroy() const
      {
        return is_marked_for_destroy;
      }

      void mark_for_destroy()
      {
        is_marked_for_destroy = true;
      }

      bool is_running            = false;
      bool is_marked_for_destroy = false;

      DeltaTime m_delta_time;
      FPS m_fps;

      rsl::unique_ptr<Window> window;

      rsl::function<bool()> on_initialize;
      rsl::function<void(const FrameInfo& info)> on_update;
      rsl::function<void()> on_shutdown;

      PlatformCreationParams platform_creation_params;
      ApplicationCreationParams app_params;
      CommandLineArguments cmd_line_args;
    };

    //-------------------------------------------------------------------------
    GuiApplication::GuiApplication(const PlatformCreationParams& platformParams, const ApplicationCreationParams& appParams, CommandLineArguments&& cmdArgs)
        : IApplication()
        , m_internal_ptr(rsl::make_unique<Internal>(platformParams, appParams, std::move(cmdArgs)))
    {
      m_internal_ptr->on_initialize = [&]() { return app_initialize(); };
      m_internal_ptr->on_update     = [&](const FrameInfo& info) { app_update(info); };
      m_internal_ptr->on_shutdown   = [&]() { app_shutdown(); };
    }

    //-------------------------------------------------------------------------
    GuiApplication::~GuiApplication() = default;

    //-------------------------------------------------------------------------
    bool GuiApplication::is_running() const
    {
      return m_internal_ptr->running() && !m_internal_ptr->marked_for_destroy();
    }

    //-------------------------------------------------------------------------
    s32 GuiApplication::run()
    {
      // Always make sure we close down the application properly
      Internal* instance = m_internal_ptr.get();
      const rsl::scopeguard shutdown_scopeguard([instance]() { instance->shutdown(); });

      if(m_internal_ptr->initialize() == false) // NOLINT(readability-simplify-boolean-expr)
      {
        REX_ERROR("Application initialization failed");
        return EXIT_FAILURE;
      }

      m_internal_ptr->update();

      return EXIT_SUCCESS;
    }

    //-------------------------------------------------------------------------
    void GuiApplication::quit()
    {
      mark_for_destroy();
    }

    //-------------------------------------------------------------------------
    void GuiApplication::mark_for_destroy()
    {
      m_internal_ptr->mark_for_destroy();
    }
  } // namespace win32
} // namespace rex
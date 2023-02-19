#include "rex_windows/gui_application.h"

#include "rex_engine/diagnostics/logging.h"
#include "rex_engine/event_system.h"
#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/frameinfo/fps.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_renderer_core/renderer.h"
#include "rex_std/bonus/utility/scopeguard.h"
#include "rex_std/math.h"
#include "rex_std/memory.h"
#include "rex_std/thread.h"
#include "rex_windows/platform_creation_params.h"
#include "rex_windows/win_window.h"

#include <Windows.h>

//NOLINTBEGIN(modernize-use-nullptr,-warnings-as-errors)

#if REX_RENDERER_OPENGL
  #include <glad/gl.h>
  #include <glad/wgl.h>

struct WglContext
{
  HDC dc;
  HGLRC glrc;
};

WglContext g_glctx;

namespace rex
{
  bool rex_make_gl_context_current()
  {
    return static_cast<bool>(wglMakeCurrent(g_glctx.dc, g_glctx.glrc));
  }
  void rex_gl_swap_buffers()
  {
    SwapBuffers(g_glctx.dc);
  }

  namespace opengl
  {
    void release_device_context(HWND hwnd, HDC dc)
    {
      // The application must call the ReleaseDC function for each call to the GetWindowDC function
      // and for each call to the GetDC function that retrieves a common DC.
      // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-releasedc
      ReleaseDC(hwnd, dc);
    }

    bool create_gl_context(HWND hwnd)
    {
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
        REX_ERROR("Failed to set a compatible pixel format");
        return false;
      }

      HGLRC temp = wglCreateContext(g_glctx.dc);
      if(temp == NULL)
      {
        release_device_context(hwnd, g_glctx.dc);
        REX_ERROR("Failed to create the initial rendering context");
        return false;
      }

      wglMakeCurrent(g_glctx.dc, temp);

      // Load WGL Extensions:
      gladLoaderLoadWGL(g_glctx.dc);

      s32 attribs[] = {WGL_CONTEXT_MAJOR_VERSION_ARB,
                       3, // Set the MAJOR version of OpenGL to 3
                       WGL_CONTEXT_MINOR_VERSION_ARB,
                       2, // Set the MINOR version of OpenGL to 2
                       WGL_CONTEXT_FLAGS_ARB,
                       WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, // Set our OpenGL context to be forward compatible
                       0};

      const char* extensions = wglGetExtensionsStringARB(g_glctx.dc);
      if(strstr(extensions, "WGL_ARB_create_context") == NULL)
      {
        g_glctx.glrc = wglCreateContextAttribsARB(g_glctx.dc, NULL, attribs);
        if(g_glctx.glrc == NULL)
        {
          wglDeleteContext(temp);
          release_device_context(hwnd, g_glctx.dc);
          REX_ERROR("Failed to create Windows OpenGL Context");

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

      if(gladLoaderLoadGL() == NULL)
      {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(g_glctx.glrc);
        release_device_context(hwnd, g_glctx.dc);
        REX_ERROR("Glad Loader failed!");

        return false;
      }

      return true;
    }
  } // namespace opengl
} // namespace rex

  #define CREATE_CTX(hwnd) rex::opengl::create_gl_context(hwnd)
#else
  #define CREATE_CTX(hwnd) (hwnd == nullptr)
#endif

namespace rex
{
  namespace win32
  {
    struct GuiApplication::Internal
    {
      Internal(const PlatformCreationParams& platformCreationParams, const GuiParams& guiParams, const CommandLineArguments& genericCreationParams)
          : platform_creation_params(platformCreationParams)
          , gui_params(guiParams)
          , cmd_line_args(genericCreationParams)
      {
      }

      bool initialize()
      {
        window = create_window();
        if(window == nullptr)
        {
          return false;
        }
        subscribe_window_events();

        HWND hwnd = (HWND)window->get_primary_display_handle();
        if(CREATE_CTX(hwnd) == false)
        {
          return false;
        }

        if(renderer::initialize((void*)&hwnd, gui_params.max_render_commands) == false)
        {
          return false;
        }

        REX_STATIC_TODO("We should find a solution to convert from OpenGL unsigned byte pointers without upsetting the LINTER");
        //RendererInfo info = renderer::get_info();
        //REX_INFO("Renderer Info - API Version: {}", info.api_version);
        //REX_INFO("Renderer Info - Adaptor: {}", info.adaptor);
        //REX_INFO("Renderer Info - Shader Version: {}", info.shader_version);
        //REX_INFO("Renderer Info - Vendor: {}", info.vendor);

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
        renderer::shutdown();
      }

      void loop()
      {
        const FrameInfo info = {m_delta_time, m_fps};

        on_update(info);

        renderer::backend::clear();
        renderer::backend::present();

        m_delta_time.update();
        m_fps.update();

        window->update();

        is_running = !is_marked_for_destroy;

        //
        // Cap framerate to "max_fps".
        // Safe resources of the machine we are running on.
        //
        const rsl::chrono::milliseconds actual_time(static_cast<int64>(rsl::lrint(1000.0f / static_cast<f32>(m_fps.get()))));
        const rsl::chrono::milliseconds desired_time(static_cast<int64>(rsl::lrint(1000.0f / static_cast<f32>(gui_params.max_fps))));

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
        wnd_description.title    = gui_params.window_title;
        wnd_description.viewport = {0, 0, gui_params.window_width, gui_params.window_height};

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
      GuiParams gui_params;
      CommandLineArguments cmd_line_args;
    };

    //-------------------------------------------------------------------------
    GuiApplication::GuiApplication(const ApplicationCreationParams& appParams)
        : CoreApplication(appParams.engine_params, appParams.cmd_args)
        , m_internal_ptr(rsl::make_unique<Internal>(appParams.platform_params, appParams.gui_params, appParams.cmd_args))
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

//NOLINTEND(modernize-use-nullptr,-warnings-as-errors)
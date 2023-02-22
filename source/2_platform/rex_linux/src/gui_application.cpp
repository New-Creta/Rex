#include "rex_linux/gui_application.h"

#include "rex_engine/diagnostics/logging.h"
#include "rex_engine/event_system.h"
#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/frameinfo/fps.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_renderer_core/renderer.h"
#include "rex_renderer_core/context.h"
#include "rex_std/bonus/utility/scopeguard.h"
#include "rex_std/math.h"
#include "rex_std/memory.h"
#include "rex_std/thread.h"
#include "rex_linux/platform_creation_params.h"
#include "rex_linux/lnx_window.h"

#include <Windows.h>

// NOLINTBEGIN(modernize-use-nullptr,-warnings-as-errors)

namespace rex
{
  namespace linux
  {
    struct GuiApplication::Internal
    {
      Internal(const PlatformCreationParams& platformCreationParams, const GuiParams& guiParams, CommandLineArguments genericCreationParams)
          : platform_creation_params(platformCreationParams)
          , gui_params(guiParams)
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
        subscribe_window_events();

        // if(context::create(window->primary_display_handle()) == false)
        // {
        //   return false;
        // }

        // if(renderer::initialize(nullptr, gui_params.max_render_commands) == false)
        // {
        //   return false;
        // }

        // RendererInfo info = renderer::info();
        // REX_INFO("Renderer Info - API Version: {}", info.api_version);
        // REX_INFO("Renderer Info - Adaptor: {}", info.adaptor);
        // REX_INFO("Renderer Info - Shader Version: {}", info.shader_version);
        // REX_INFO("Renderer Info - Vendor: {}", info.vendor);

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
      void shutdown() // NOLINT (readability-make-member-function-const,-warnings-as-errors)
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

        if(wnd->create(wnd_description))
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

// NOLINTEND(modernize-use-nullptr,-warnings-as-errors)
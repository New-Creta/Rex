#include "rex_windows/gui_application.h"

#include "rex_engine/core_window.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/event.h" // IWYU pragma: keep
#include "rex_engine/event_system.h"
#include "rex_engine/event_type.h"
#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/frameinfo/fps.h"
#include "rex_renderer_core/context.h"
#include "rex_renderer_core/renderer.h"
#include "rex_std/bonus/types.h"
#include "rex_std/functional.h"
#include "rex_std/math.h"
#include "rex_std/memory.h"
#include "rex_std/ratio.h"
#include "rex_std/thread.h"
#include "rex_std/vector.h"
#include "rex_windows/log.h"
#include "rex_windows/platform_creation_params.h"
#include "rex_windows/win_window.h"

// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
// NOLINTBEGIN(modernize-use-nullptr)

namespace rex
{

  namespace win32
  {
    class GuiApplication::Internal
    {
    public:
      Internal(CoreApplication* appInstance, ApplicationCreationParams&& appCreationParams)
          : m_platform_creation_params(rsl::move(appCreationParams.platform_params))
          , m_gui_params(rsl::move(appCreationParams.gui_params))
          , m_cmd_line_args(rsl::move(appCreationParams.cmd_args))
          , m_engine_params(rsl::move(appCreationParams.engine_params))
          , m_app_instance(appInstance)
      {
        // we're always assigning something to the pointers here to avoid branch checking every update
        // I've profiled this and always having a function wins here.
        m_on_initialize = m_engine_params.app_init_func ? m_engine_params.app_init_func : [&]() { return true; };

        m_on_update = m_engine_params.app_update_func ? m_engine_params.app_update_func : [&]() {};

        m_on_shutdown = m_engine_params.app_shutdown_func ? m_engine_params.app_shutdown_func : [&]() {};
      }

      bool initialize()
      {
        REX_ASSERT_CONTEXT_SCOPE("Application initialization");

        // window initialization
        m_window = create_window();
        if(m_window == nullptr)
        {
          return false;
        }
        subscribe_window_events();

        // graphics context initialization
        if(context::create(m_window->primary_display_handle()) == false) // NOLINT(readability-simplify-boolean-expr)
        {
          return false;
        }

        // renderer initialization
        if(renderer::initialize(nullptr, m_gui_params.max_render_commands) == false) // NOLINT(readability-simplify-boolean-expr)
        {
          return false;
        }

        display_renderer_info();

        // call client code so it can get initialized
        return m_on_initialize();
      }
      void update()
      {
        REX_ASSERT_CONTEXT_SCOPE("Application update");

        // update the window (this pulls input as well)
        m_window->update();

        if(!m_app_instance->is_paused())
        {
          // call the client code, let it update
          m_on_update();

          // update the graphics code
          renderer::backend::clear();
          renderer::backend::present();

          // update the timing stats
          m_delta_time.update();
          m_fps.update();

          ++m_frame_idx;
        }

        cap_frame_rate();
      }
      void shutdown() // NOLINT (readability-make-member-function-const,-warnings-as-errors)
      {
        REX_ASSERT_CONTEXT_SCOPE("Application shutdown");

        m_on_shutdown();
        renderer::shutdown();
      }

    private:
      rsl::unique_ptr<Window> create_window()
      {
        auto wnd = rsl::make_unique<Window>();

        WindowDescription wnd_description;
        wnd_description.title    = m_gui_params.window_title.to_string_view();
        wnd_description.viewport = {0, 0, m_gui_params.window_width, m_gui_params.window_height};

        if(wnd->create(m_platform_creation_params.instance, m_platform_creation_params.show_cmd, wnd_description))
        {
          return wnd;
        }

        REX_ERROR(LogWindows, "Window was not created returning nullptr");
        return nullptr;
      }

      void subscribe_window_events()
      {
        event_system::subscribe(event_system::EventType::WindowClose, [this](const event_system::Event& /*evt*/) { m_app_instance->quit(); });
        event_system::subscribe(event_system::EventType::WindowActivate, [this](const event_system::Event& /*evt*/) { m_app_instance->resume(); });
        event_system::subscribe(event_system::EventType::WindowDeactivate, [this](const event_system::Event& /*evt*/) { m_app_instance->pause(); });
        event_system::subscribe(event_system::EventType::WindowStartWindowResize, [this](const event_system::Event& /*evt*/) { on_start_resize(); });
        event_system::subscribe(event_system::EventType::WindowStopWindowResize, [this](const event_system::Event& evt) { on_stop_resize(evt); });
        event_system::subscribe(event_system::EventType::WindowMinimized, [this](const event_system::Event& /*evt*/) { on_minimize(); });
        event_system::subscribe(event_system::EventType::WindowMaximized, [this](const event_system::Event& evt) { on_maximize(evt); });
        event_system::subscribe(event_system::EventType::WindowRestored, [this](const event_system::Event& evt) { on_restore(evt); });
      }

      void display_renderer_info() // NOLINT(readability-convert-member-functions-to-static)
      {
        RendererInfo info = renderer::info();
        REX_LOG(LogWindows, "Renderer Info - API Version: {}", info.api_version);
        REX_LOG(LogWindows, "Renderer Info - Adaptor: {}", info.adaptor);
        REX_LOG(LogWindows, "Renderer Info - Shader Version: {}", info.shader_version);
        REX_LOG(LogWindows, "Renderer Info - Vendor: {}", info.vendor);
      }

      void cap_frame_rate()
      {
        //
        // Cap framerate to "max_fps".
        // Safe resources of the machine we are running on.
        //
        const rsl::chrono::milliseconds actual_time(static_cast<int64>(rsl::lrint(1000.0f / static_cast<f32>(m_fps.get()))));
        const rsl::chrono::milliseconds desired_time(static_cast<int64>(rsl::lrint(1000.0f / static_cast<f32>(m_gui_params.max_fps))));

        const rsl::chrono::duration<float> elapsed_time = desired_time - actual_time;
        using namespace rsl::chrono_literals; // NOLINT(google-build-using-namespace)
        if(elapsed_time > 0ms)
        {
          rsl::this_thread::sleep_for(elapsed_time);
        }
      }

      void on_start_resize()
      {
        m_app_instance->pause();
        m_window->start_resize();
      }

      void on_stop_resize(const event_system::Event& evt)
      {
        m_app_instance->resume();
        m_window->stop_resize();

        REX_ASSERT_X(evt.type == event_system::EventType::WindowStopWindowResize, "Event has to be of type \"WindowStopWindowResize\"");

        resize(evt);
      }

      void on_minimize()
      {
        m_app_instance->pause();
        m_window->minimize();
      }

      void on_maximize(const event_system::Event& evt)
      {
        m_app_instance->resume();
        m_window->maximize();

        REX_ASSERT_X(evt.type == event_system::EventType::WindowMaximized, "Event has to be of type \"WindowMaximized\"");

        resize(evt);
      }

      void on_restore(const event_system::Event& evt)
      {
        REX_ASSERT_X(evt.type == event_system::EventType::WindowRestored, "Event has to be of type \"WindowRestored\"");

        if(m_window->is_minimized())
        {
          m_app_instance->resume();
          m_window->restore();

          resize(evt);
        }
        else if(m_window->is_maximized())
        {
          m_window->restore();

          resize(evt);
        }
        else if(m_window->is_resizing())
        {
          // If user is dragging the resize bars, we do not resize
          // the buffers here because as the user continuously
          // drags the resize bars, a stream of "Resizing" messages are
          // sent to the window, and it would be pointless (and slow)
          // to resize for each "Resizing" message received from dragging
          // the resize bars.
          //
          // So instead, we reset after the user is done resizing the
          // window and releases the resize bars, which sends a
          // "WindowStopWindowResize" message.
        }
        else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
        {
          REX_WARN(LogWindows, "API call such as SetWindowPos or mSwapChain->SetFullscreenState will also invoke a resize event.");
        }
      }

      void resize(const event_system::Event& /*evt*/)
      {
        // Resize window ( although we might want to capture this within the window itself ... )
        //
        // Flush Command Queue
        //
        // Release front and back buffer
        // Release depth stencil buffer
        //
        // Reset current back buffer back to first entry ( aka 0 )
        //
        // Recreate swap chain buffers
        // Recreate depth stencil buffer
        //
        // Transition depth stencil from it's initial state to be used as a depth buffer (DX only)
        //
        // Execute all resize commands
        //
        // Wait until completed
        //
        // Update screen viewport
        // Update scissor rect
      }

    private:
      DeltaTime m_delta_time;
      FPS m_fps;

      rsl::unique_ptr<Window> m_window;

      rsl::function<bool()> m_on_initialize;
      rsl::function<void()> m_on_update;
      rsl::function<void()> m_on_shutdown;

      PlatformCreationParams m_platform_creation_params;
      GuiParams m_gui_params;
      CommandLineArguments m_cmd_line_args;
      EngineParams m_engine_params;
      CoreApplication* m_app_instance;

      card32 m_frame_idx = 0;
    };

    //-------------------------------------------------------------------------
    GuiApplication::GuiApplication(ApplicationCreationParams&& appParams)
        : CoreApplication(appParams.engine_params, appParams.cmd_args)
        , m_internal_ptr(rsl::make_unique<Internal>(this, rsl::move(appParams)))
    {
    }

    //-------------------------------------------------------------------------
    GuiApplication::~GuiApplication() = default;

    //--------------------------------------------------------------------------------------------
    bool GuiApplication::platform_init()
    {
      return m_internal_ptr->initialize();
    }
    void GuiApplication::platform_update()
    {
      m_internal_ptr->update();
    }
    void GuiApplication::platform_shutdown()
    {
      m_internal_ptr->shutdown();
    }

  } // namespace win32
} // namespace rex

// NOLINTEND(modernize-use-nullptr)
// NOLINTEND(cppcoreguidelines-pro-type-union-access)
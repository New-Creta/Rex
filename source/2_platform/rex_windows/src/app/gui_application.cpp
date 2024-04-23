﻿#include "rex_windows/app/gui_application.h"

#include "rex_engine/app/core_window.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/event_system/event.h" // IWYU pragma: keep
#include "rex_engine/event_system/event_system.h"
#include "rex_engine/event_system/event_type.h"
#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/frameinfo/fps.h"
#include "rex_engine/platform/win/win_com_library.h"
#include "rex_engine/scenegraph/scene_manager.h"
#include "rex_engine/scenegraph/scene_node.h"
#include "rex_engine/windowinfo.h"
#include "rex_renderer_core/renderer.h"
#include "rex_renderer_core/renderer_info.h"
#include "rex_renderer_core/renderer_output_window_user_data.h"
#include "rex_std/bonus/types.h"
#include "rex_std/functional.h"
#include "rex_std/math.h"
#include "rex_std/memory.h"
#include "rex_std/ratio.h"
#include "rex_std/thread.h"
#include "rex_windows/app/win_window.h"
#include "rex_windows/diagnostics/log.h"
#include "rex_windows/engine/platform_creation_params.h"

// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
// NOLINTBEGIN(modernize-use-nullptr)

namespace rex
{
  namespace globals
  {
    WindowInfo g_window_info; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)S

    //-------------------------------------------------------------------------
    const WindowInfo& window_info()
    {
      return g_window_info;
    }
  } // namespace globals
  namespace win
  {
    class GuiApplication::Internal
    {
    public:
      Internal(CoreApplication* appInstance, ApplicationCreationParams&& appCreationParams)
          : m_platform_creation_params(*appCreationParams.platform_params)
          , m_gui_params(rsl::move(appCreationParams.gui_params))
          , m_engine_params(rsl::move(appCreationParams.engine_params))
          , m_app_instance(appInstance)
      {
        // we're always assigning something to the pointers here to avoid branch checking every update
        // I've profiled this and always having a function wins here.
        m_on_initialize = m_engine_params.app_init_func ? rsl::move(m_engine_params.app_init_func) : [&]() { return true; };

        m_on_update = m_engine_params.app_update_func ? rsl::move(m_engine_params.app_update_func) : [&]() {};

        m_on_gui = m_engine_params.app_gui_func ? rsl::move(m_engine_params.app_gui_func) : [&]() {};

        m_on_shutdown = m_engine_params.app_shutdown_func ? rsl::move(m_engine_params.app_shutdown_func) : [&]() {};
      }

      bool initialize()
      {
        REX_ASSERT_CONTEXT_SCOPE("Application initialization");

        // initialize the com lib
        // Doing this very early on so other system can use the com lib
        // for their initialization if needed
        m_win_com_lib_handle = win::com_lib::create_lib_handle();

        // window initialization
        m_window = create_window();
        if(m_window == nullptr)
        {
          return false;
        }

        globals::g_window_info.width  = m_window->width();
        globals::g_window_info.height = m_window->height();

        subscribe_window_events();

        // scene setup
        SceneManager::create_instance();
        SceneManager::instance()->active_scene()->set_root_node(rsl::make_shared<SceneNode>(glm::mat4(1.0), rsl::string("Root Node")));

        // renderer initialization
        renderer::OutputWindowUserData user_data {};
        user_data.primary_display_handle = m_window->primary_display_handle();
        user_data.refresh_rate           = m_gui_params.max_fps;
        user_data.window_width           = m_window->width();
        user_data.window_height          = m_window->height();
        user_data.windowed               = !m_gui_params.fullscreen;

        if(renderer::initialize(user_data) == false) // NOLINT(readability-simplify-boolean-expr)
        {
          return false;
        }

        display_renderer_info();

        // call client code so it can get initialized
        if (!m_on_initialize())
        {
          return false;
        }

        if(renderer::post_initialize() == false) // NOLINT(readability-simplify-boolean-expr)
        {
          return false;
        }

        renderer::flush();

        // When the renderer is initialized we can show the window
        m_window->show();

        return true;
      }
      void update()
      {
        REX_ASSERT_CONTEXT_SCOPE("Application update");

        // pull new events
        m_window->poll_events();
            
        // process the received events
        event_system::process_events();

        // update the timing stats
        m_delta_time.update();
        m_fps.update();

        cap_frame_rate();
      }
      void shutdown() // NOLINT (readability-make-member-function-const,-warnings-as-errors)
      {
        REX_ASSERT_CONTEXT_SCOPE("Application shutdown");

        m_on_shutdown();
        renderer::shutdown();
        SceneManager::destroy_instance();
      }

    private:
      rsl::unique_ptr<Window> create_window()
      {
        auto wnd = rsl::make_unique<Window>();

        WindowDescription wnd_description;
        wnd_description.title    = m_gui_params.window_title;
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
        event_system::subscribe(event_system::EventType::WindowClose, [this](const event_system::Event& /*evt*/) { event_system::enqueue_event(event_system::EventType::QuitApp); });
        event_system::subscribe(event_system::EventType::WindowActivate, [this](const event_system::Event& /*evt*/) { m_app_instance->resume(); });
        event_system::subscribe(event_system::EventType::WindowDeactivate, [this](const event_system::Event& /*evt*/) { m_app_instance->pause(); });
        event_system::subscribe(event_system::EventType::WindowStartWindowResize, [this](const event_system::Event& /*evt*/) { on_start_resize(); });
        event_system::subscribe(event_system::EventType::WindowStopWindowResize, [this](const event_system::Event& evt) { on_stop_resize(evt); });
        event_system::subscribe(event_system::EventType::WindowMinimized, [this](const event_system::Event& /*evt*/) { on_minimize(); });
        event_system::subscribe(event_system::EventType::WindowMaximized, [this](const event_system::Event& evt) { on_maximize(evt); });
        event_system::subscribe(event_system::EventType::WindowRestored, [this](const event_system::Event& evt) { on_restore(evt); });
        event_system::subscribe(event_system::EventType::Update, [this](const event_system::Event& evt) { on_update(evt); });
        event_system::subscribe(event_system::EventType::Render, [this](const event_system::Event& evt) { on_render(evt); });
        event_system::subscribe(event_system::EventType::QuitApp, [this](const event_system::Event& /*evt*/) { m_app_instance->quit(); });
      }

      void display_renderer_info() // NOLINT(readability-convert-member-functions-to-static)
      {
        renderer::Info info = renderer::info();
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

      void on_update(const event_system::Event& /*evt*/)
      {
        m_on_update();
      }

      void on_render(const event_system::Event& /*evt*/)
      {
        // don't render when the application is paused
        if(!m_app_instance->is_paused())
        {
          renderer::draw_scene(SceneManager::instance()->active_scene());

          m_on_gui();

          renderer::draw_gui();
          renderer::present();
        }
      }

    private:
      DeltaTime m_delta_time;
      FPS m_fps;

      rsl::unique_ptr<Window> m_window;

      rsl::function<bool()> m_on_initialize;
      rsl::function<void()> m_on_update;
      rsl::function<void()> m_on_gui;
      rsl::function<void()> m_on_shutdown;

      PlatformCreationParams m_platform_creation_params;
      GuiParams m_gui_params;
      EngineParams m_engine_params;
      CoreApplication* m_app_instance;
      win::com_lib::WinComLibHandle m_win_com_lib_handle;
    };

    //-------------------------------------------------------------------------
    GuiApplication::GuiApplication(ApplicationCreationParams&& appParams)
        : CoreApplication(appParams.engine_params)
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

  } // namespace win
} // namespace rex

// NOLINTEND(modernize-use-nullptr)
// NOLINTEND(cppcoreguidelines-pro-type-union-access)
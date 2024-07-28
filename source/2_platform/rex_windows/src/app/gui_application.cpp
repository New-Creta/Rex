#include "rex_windows/app/gui_application.h"

#include "rex_engine/app/core_window.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/event_system/event.h" // IWYU pragma: keep
#include "rex_engine/event_system/event_system.h"
#include "rex_engine/frameinfo/deltatime.h"
#include "rex_engine/frameinfo/fps.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_engine/platform/win/win_com_library.h"
#include "rex_renderer_core/gfx/depth_info.h"
#include "rex_renderer_core/gfx/renderer_output_window_user_data.h"
#include "rex_renderer_core/gfx/rhi.h"
#include "rex_renderer_core/gfx/graphics.h"
#include "rex_std/bonus/types.h"
#include "rex_std/functional.h"
#include "rex_std/math.h"
#include "rex_std/memory.h"
#include "rex_std/ratio.h"
#include "rex_std/thread.h"
#include "rex_windows/app/win_window.h"
#include "rex_windows/diagnostics/log.h"
#include "rex_windows/engine/platform_creation_params.h"

#include "rex_engine/event_system/events/window/window_close.h"
#include "rex_engine/event_system/events/window/window_activated.h"
#include "rex_engine/event_system/events/window/window_deactivated.h"
#include "rex_engine/event_system/events/window/window_start_resize.h"
#include "rex_engine/event_system/events/window/window_end_resize.h"
#include "rex_engine/event_system/events/window/window_resize.h"
#include "rex_engine/event_system/events/app/quit_app.h"
#include "rex_engine/settings/settings.h"

#include "rex_renderer_core/imgui/imgui_renderer.h"

#include "rex_renderer_core/gfx/graphics.h"
#include "rex_engine/diagnostics/debug.h"
#include "rex_engine/memory/tracked_allocator.h"

// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
// NOLINTBEGIN(modernize-use-nullptr)

namespace rex
{
  namespace win
  {
    class GuiApplication::Internal
    {
    public:
      Internal(CoreApplication* appInstance, ApplicationCreationParams&& appCreationParams)
          : m_app_creation_params(rsl::move(appCreationParams))
          , m_app_instance(appInstance)
      {
        // we're always assigning something to the pointers here to avoid branch checking every update
        // I've profiled this and always having a function wins here.
        m_on_initialize = m_app_creation_params.engine_params.app_init_func ? rsl::move(m_app_creation_params.engine_params.app_init_func) : [&](const ApplicationCreationParams&) { return true; };

        m_on_update = m_app_creation_params.engine_params.app_update_func ? rsl::move(m_app_creation_params.engine_params.app_update_func) : [&]() {};

        m_on_shutdown = m_app_creation_params.engine_params.app_shutdown_func ? rsl::move(m_app_creation_params.engine_params.app_shutdown_func) : [&]() {};
      }

      bool initialize()
      {
        REX_ASSERT_CONTEXT_SCOPE("Application initialization");

        bool result = pre_user_init();

        if(!result)
        {
          REX_ERROR(LogWindows, "Pre user initialization failed. exiting");
          return result;
        }

        // call client code so it can get initialized
        result = m_on_initialize(m_app_creation_params);

        if(!result)
        {
          REX_ERROR(LogWindows, "User initialization failed. exiting.");
          return result;
        }

        result = post_user_init();
        if(!result)
        {
          REX_ERROR(LogWindows, "Post user initialization failed. exiting.");
          return result;
        }

        return result;
      }
      void update()
      {
        REX_ASSERT_CONTEXT_SCOPE("Application update");

        pre_user_update();

        m_on_update();

        post_user_update();

        // update the timing stats

        m_delta_time.update();
        m_fps.update();

        cap_frame_rate();
      }
      void draw()
      {
        // don't render when the application gets destroyed soon
        if (m_app_instance->is_marked_for_destroy())
        {
          return;
        }

        // Call into our graphics API to render everything
        gfx::render();
      }
      void shutdown() // NOLINT (readability-make-member-function-const,-warnings-as-errors)
      {
        REX_ASSERT_CONTEXT_SCOPE("Application shutdown");

        m_on_shutdown();

        gfx::shutdown();
      }

    private:
      void cap_frame_rate()
      {
        //
        // Cap framerate to "max_fps".
        // Safe resources of the machine we are running on.
        //
        const rsl::chrono::milliseconds actual_time(static_cast<int64>(rsl::lrint(1000.0f / static_cast<f32>(m_fps.get()))));
        const rsl::chrono::milliseconds desired_time(static_cast<int64>(rsl::lrint(1000.0f / static_cast<f32>(m_app_creation_params.gui_params.max_fps))));

        const rsl::chrono::duration<float> elapsed_time = desired_time - actual_time;
        using namespace rsl::chrono_literals; // NOLINT(google-build-using-namespace)
        if(elapsed_time > 0ms)
        {
          rsl::this_thread::sleep_for(elapsed_time);
        }
      }

      // Window resizing
      void on_start_resize()
      {
        m_app_instance->pause();
        m_window->start_resize();
      }
      void on_stop_resize()
      {
        m_app_instance->resume();
        m_window->stop_resize();
      }
      void on_minimize()
      {
        m_app_instance->pause();
        m_window->minimize();
      }
      void on_maximize(s32 newWidth, s32 newHeight)
      {
        m_app_instance->resume();
        m_window->maximize();

        resize(newWidth, newHeight);
      }
      void on_restore(s32 newWidth, s32 newHeight)
      {
        if(m_window->is_minimized())
        {
          m_app_instance->resume();
          m_window->restore();

          resize(newWidth, newHeight);
        }
        else if(m_window->is_maximized())
        {
          m_window->restore();

          resize(newWidth, newHeight);
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
        else // API call such as SetWindowPos or mSwapChain->SetFullscreenState will also invoke a resize event.
        {
          // Do nothing
        }
      }
      void resize(s32 /*newWidth*/, s32 /*newHeight*/)
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

      // Initialization
      bool pre_user_init()
      {
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

        subscribe_window_events();

        output_debug_string("Memory usage before graphics initialization");
        log_mem_usage();

        if(!init_gfx())
        {
          REX_ERROR(LogWindows, "Failed to initialize graphics");
          return false;
        }

        output_debug_string("Memory usage after graphics initialization");
        log_mem_usage();

        return true;
      }
      bool post_user_init()
      {
        // When the renderer is initialized we can show the window
        m_window->show();

        return true;
      }
      rsl::unique_ptr<Window> create_window()
      {
        auto wnd = rsl::make_unique<Window>();

        WindowInfo window_info;
        window_info.title    = m_app_creation_params.gui_params.window_title;
        window_info.viewport = {0, 0, m_app_creation_params.gui_params.window_width, m_app_creation_params.gui_params.window_height};

        if(wnd->create(m_app_creation_params.platform_params->instance, m_app_creation_params.platform_params->show_cmd, window_info))
        {
          return wnd;
        }

        REX_ERROR(LogWindows, "Window was not created returning nullptr");
        return nullptr;
      }
      void subscribe_window_events()
      {
        event_system().subscribe<WindowClose>([this](const WindowClose& /*evt*/) { event_system().enqueue_event(QuitApp()); });
        event_system().subscribe<WindowActivated>( [this](const WindowActivated& /*evt*/) { m_app_instance->resume(); });
        event_system().subscribe<WindowDeactivated>( [this](const WindowDeactivated& /*evt*/) { m_app_instance->pause(); });
        event_system().subscribe<WindowStartResize>( [this](const WindowStartResize& /*evt*/) { on_start_resize(); });
        event_system().subscribe<WindowEndResize>( [this](const WindowEndResize& /*evt*/) { on_stop_resize(); });
        event_system().subscribe<QuitApp>( [this](const QuitApp& /*evt*/) { m_app_instance->quit(); });
        event_system().subscribe<WindowResize>( [this](const WindowResize& evt) 
          { 
            switch (evt.resize_type())
            {
            case WindowResizeType::Maximized:
              on_maximize(evt.width(), evt.height());
              break;
            case WindowResizeType::Minimized:
              on_minimize();
              break;
            case WindowResizeType::Restored:
              on_restore(evt.width(), evt.height());
              break;
            default:
              break;
            }
          });
      }

      // Initialize the graphics pipeline
      // Allocating memory and setting up the resources
      // to start using the graphics pipeline.
      bool init_gfx()
      {
        gfx::OutputWindowUserData user_data {};
        user_data.primary_display_handle = m_window->primary_display_handle();
        user_data.refresh_rate           = m_app_creation_params.gui_params.max_fps;
        user_data.window_width           = m_window->width();
        user_data.window_height          = m_window->height();
        user_data.windowed               = !m_app_creation_params.gui_params.fullscreen;
        user_data.max_frames_in_flight   = settings::get_int("max_frames_in_flight", 3);

        gfx::init(user_data);

        REX_WARN_ONCE(LogWindows, "Create the viewport manager here");

        // Add the imgui renderer, which is our main UI renderer for the moment
        gfx::add_renderer(rsl::make_unique<gfx::ImGuiRenderer>(m_window->primary_display_handle()));

        return true;
      }

      // Updating
      void pre_user_update()
      {
        // update timers
        REX_WARN_ONCE(LogWindows, "Create an app clock");

        // update the window (this pulls input as well)
        m_window->update();

        // Dispatch the events that got queued last frame
        event_system().dispatch_queued_events();
      }
      void post_user_update()
      {
        // Nothing to implement
      }

    private:
      DeltaTime m_delta_time;
      FPS m_fps;

      rsl::unique_ptr<Window> m_window;

      rsl::function<bool(const ApplicationCreationParams&)> m_on_initialize;
      rsl::function<void()> m_on_update;
      rsl::function<void()> m_on_draw;
      rsl::function<void()> m_on_shutdown;

      ApplicationCreationParams m_app_creation_params;
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
      // At the moment we update and render on the same thread
      // Meaning we first update all our data
      // Afterwards we sent this updated data to the renderer
      // In most cases these will just be object positions
      // In other cases this will be transition from one level into another

      m_internal_ptr->update();
      m_internal_ptr->draw();
    }
    void GuiApplication::platform_shutdown()
    {
      m_internal_ptr->shutdown();
    }

  } // namespace win
} // namespace rex

  // NOLINTEND(modernize-use-nullptr)
// NOLINTEND(cppcoreguidelines-pro-type-union-access)
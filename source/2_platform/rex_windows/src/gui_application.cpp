#include "rex_windows/gui_application.h"
#include "rex_windows/win_window.h"

#include "rex_engine/core_application_params.h"
#include "rex_engine/event_system.h"
#include "rex_engine/world.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_engine/diagnostics/logging.h"

#include "rex_std/memory.h"
#include "rex_std/math.h"

#include "rex_std_extra/utilities/scopeguard.h"

namespace rex
{
  namespace win32
  {
    struct GuiApplicationCreationParamters
    {
      HInstance h_instance;
      HInstance h_prev_instance;
      LPtStr cmd_line;
      s32 cmd_show;
    };

    struct GuiApplication::Internal
    {
      Internal(GuiApplicationCreationParamters&& guiCreationParams, ApplicationCreationParams&& genericCreationParams)
        :gui_creation_params(std::move(guiCreationParams))
        ,generic_creation_params(std::move(genericCreationParams))
      {}

      bool initialize()
      {
        window = create_window();
        if (window == nullptr)
        {
          return false;
        }

        return on_initialize();
      }
      void update()
      {
        is_running = true;

        while (is_running)
        {
          loop();

          is_running = !is_marked_for_destroy;
        }
      }
      void shutdown()
      {
        on_shutdown();

        if (window)
        {
          window->destroy();
        }
      }

      void loop()
      {
        FrameInfo info = { World::getDeltaTime(), World::getFramesPerSecond() };

        on_update(info);

        window->update();
        world.update();

        is_running = !is_marked_for_destroy;

        //
        // Cap framerate to "max_fps".
        // Safe resources of the machine we are running on.
        //
        rsl::chrono::milliseconds actual_time(static_cast<int>(rsl::lrint(1000.0f / world.getFramesPerSecond().get())));
        rsl::chrono::milliseconds desired_time(static_cast<int>(rsl::lrint(1000.0f / generic_creation_params.max_fps)));

        rsl::chrono::duration<float> elapsed_time = desired_time - actual_time;
        if (elapsed_time > rsl::chrono::milliseconds(0ms))
        {
          //std::this_thread::sleep_for(elapsed_time);
        }
      }

      rsl::unique_ptr<Window> create_window()
      {
        auto wnd = rsl::make_unique<Window>();

        WindowDescription wnd_description;
        wnd_description.title = generic_creation_params.window_title;
        wnd_description.viewport = { 0,0, generic_creation_params.window_width, generic_creation_params.window_height };

        if (wnd->create(gui_creation_params.h_instance, gui_creation_params.cmd_show, wnd_description))
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

      bool is_running = false;
      bool is_marked_for_destroy = false;

      World world;

      rsl::unique_ptr<Window> window;

      rsl::function<bool()> on_initialize;
      rsl::function<void(const FrameInfo& info)> on_update;
      rsl::function<void()> on_shutdown;

      GuiApplicationCreationParamters gui_creation_params;
      ApplicationCreationParams generic_creation_params;
    };

    //-------------------------------------------------------------------------
    GuiApplication::GuiApplication(HInstance hInstance, HInstance hPrevInstance, LPtStr lpCmdLine, s32 nCmdShow, ApplicationCreationParams&& creationParams)
      :IApplication()
      , m_internal_ptr(rsl::make_unique<Internal>(GuiApplicationCreationParamters{ hInstance, hPrevInstance, lpCmdLine, nCmdShow }, std::move(creationParams)))
    {
      m_internal_ptr->on_initialize = [&]() { return app_initialize(); };
      m_internal_ptr->on_update = [&](const FrameInfo& info) { app_update(info); };
      m_internal_ptr->on_shutdown = [&]() { app_shutdown(); };
    }

    //-------------------------------------------------------------------------
    GuiApplication::~GuiApplication()
    {}

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
      rsl::ScopeGuard shutdown_scopeguard([instance]() { instance->shutdown(); });

      if (m_internal_ptr->initialize() == false)
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
  }
}
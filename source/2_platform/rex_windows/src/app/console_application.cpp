#include "rex_windows/app/console_application.h"

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/engine/engine_params.h"
#include "rex_engine/event_system/event_system.h"
#include "rex_std/functional.h"
#include "rex_windows/engine/platform_creation_params.h"
#include "rex_windows/input/internal/input.h"

#include "rex_engine/event_system/events/app/quit_app.h"

#include <Windows.h>
#include <consoleapi.h>
#include <consoleapi3.h>

namespace rex
{
  namespace win
  {
    DEFINE_LOG_CATEGORY(LogWinConsoleApp);

    CoreApplication* g_this_app = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

    BOOL WINAPI handler_routine(DWORD eventCode)
    {
      switch(eventCode) // NOLINT(hicpp-multiway-paths-covered)
      {
          // you only have 5 seconds to process this
          // Windows forcefully kills the app after 5 seconds
          // see https://learn.microsoft.com/en-us/windows/console/handlerroutine#timeouts
        case CTRL_CLOSE_EVENT:
          g_this_app->quit();

          // Start an infinite loop so the main thread has time to shutdown the app (knowing it only has 5 seconds)
          // We cannot yield the thread here as that'd immediately kill the process without giving the main thread
          // a chance to shutdown the app properly.
          while(true)
          {
            // Do nothing here
          }

          return true; // NOLINT(readability-implicit-bool-conversion)
          break;
      }

      return false; // NOLINT(readability-implicit-bool-conversion)
    }

    class ConsoleApplication::Internal
    {
    public:
      Internal(CoreApplication* appInstance, ApplicationCreationParams&& appCreationParams)
          : m_platform_creation_params(*appCreationParams.platform_params)
          , m_engine_params(rsl::move(appCreationParams.engine_params))
          , m_app_instance(appInstance)
      {
        g_this_app = m_app_instance;

        // we're always assigning something to the pointers here to avoid branch checking every update
        // I've profiled this and always having a function wins here.
        m_on_initialize = m_engine_params.app_init_func ? rsl::move(m_engine_params.app_init_func) : [&]() { return true; };

        m_on_update = m_engine_params.app_update_func ? rsl::move(m_engine_params.app_update_func) : [&]() {};

        m_on_shutdown = m_engine_params.app_shutdown_func ? rsl::move(m_engine_params.app_shutdown_func) : [&]() {};
      }

      bool initialize()
      {
        SetConsoleCtrlHandler(handler_routine, true); // NOLINT(readability-implicit-bool-conversion)
        input::internal::set_global_input_handler(m_input);

        event_system().subscribe<QuitApp>([this](const QuitApp& /*event*/) { m_app_instance->quit(); });

        return m_on_initialize();
      }

      void update()
      {
        if(is_focussed())
        {
          m_input.update();
        }

        m_on_update();

        event_system().dispatch_queued_events();
      }

      void shutdown()
      {
        m_on_shutdown();
      }

      bool is_focussed() const // NOLINT(readability-convert-member-functions-to-static)
      {
        return GetConsoleWindow() == GetForegroundWindow();
      }

    private:
      PlatformCreationParams m_platform_creation_params;
      EngineParams m_engine_params;
      CoreApplication* m_app_instance;

      rsl::function<bool()> m_on_initialize;
      rsl::function<void()> m_on_update;
      rsl::function<void()> m_on_shutdown;

      input::internal::Input m_input;
    };

    ConsoleApplication::ConsoleApplication(ApplicationCreationParams&& appParams)
        : CoreApplication(appParams.engine_params)
        , m_internal(rsl::make_unique<ConsoleApplication::Internal>(this, rsl::move(appParams)))
    {
    }

    ConsoleApplication::~ConsoleApplication() = default;

    //--------------------------------------------------------------------------------------------
    bool ConsoleApplication::platform_init()
    {
      return m_internal->initialize();
    }
    void ConsoleApplication::platform_update()
    {
      m_internal->update();
    }
    void ConsoleApplication::platform_shutdown()
    {
      m_internal->shutdown();
    }
  } // namespace win
} // namespace rex
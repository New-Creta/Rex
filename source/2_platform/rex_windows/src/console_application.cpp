#include "rex_windows/console_application.h"

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/event_system.h"

#include <Windows.h>

namespace rex
{
  namespace win32
  {
    DEFINE_LOG_CATEGORY(LogWinConsoleApp, rex::LogVerbosity::Log);

    CoreApplication* g_this_app = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables, cppcoreguidelines-avoid-non-const-global-variables)

    BOOL WINAPI handler_routine(DWORD eventCode)
    {
      switch(eventCode) // NOLINT(hicpp-multiway-paths-covered)
      {
          // you only have 5 seconds to process this
          // Windows forcefully kills the app after 5 seconds
          // see https://learn.microsoft.com/en-us/windows/console/handlerroutine#timeouts
        case CTRL_CLOSE_EVENT:
          g_this_app->quit();
          return false; // NOLINT(readability-implicit-bool-conversion)
          break;
      }

      return true; // NOLINT(readability-implicit-bool-conversion)
    }

    class ConsoleApplication::Internal
    {
    public:
      Internal(CoreApplication* appInstance, ApplicationCreationParams&& appCreationParams)
          : m_platform_creation_params(rsl::move(appCreationParams.platform_params))
          , m_cmd_line_args(rsl::move(appCreationParams.cmd_args))
          , m_engine_params(rsl::move(appCreationParams.engine_params))
          , m_app_instance(appInstance)
      {
        g_this_app = m_app_instance;

        // we're always assigning something to the pointers here to avoid branch checking every update
        // I've profiled this and always having a function wins here.
        m_on_initialize = m_engine_params.app_init_func ? m_engine_params.app_init_func : [&]() { return true; };

        m_on_update = m_engine_params.app_update_func ? m_engine_params.app_update_func : [&]() {};

        m_on_shutdown = m_engine_params.app_shutdown_func ? m_engine_params.app_shutdown_func : [&]() {};
      }

      bool initialize()
      {
        constexpr bool is_adding = true;
        SetConsoleCtrlHandler(handler_routine, is_adding); // NOLINT(readability-implicit-bool-conversion)

        event_system::subscribe(event_system::EventType::QuitApp, [this](const event_system::Event& /*event*/) { m_app_instance->quit(); });

        return m_on_initialize();
      }

      void update()
      {
        m_on_update();
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
      CommandLineArguments m_cmd_line_args;
      EngineParams m_engine_params;
      CoreApplication* m_app_instance;

      rsl::function<bool()> m_on_initialize;
      rsl::function<void()> m_on_update;
      rsl::function<void()> m_on_shutdown;
    };

    ConsoleApplication::ConsoleApplication(ApplicationCreationParams&& appParams)
        : CoreApplication(appParams.engine_params, appParams.cmd_args)
        , m_internal(nullptr)
    {
      m_internal = rsl::make_unique<ConsoleApplication::Internal>(this, rsl::move(appParams));
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
  } // namespace win32
} // namespace rex
#include "rex_windows/console_application.h"
#include "rex_engine/event_system.h"
#include "rex_windows/input/input.h"

namespace rex
{
  namespace win32
  {
    class ConsoleApplication::Internal
    {
    public:
      Internal(CoreApplication* appInstance, ApplicationCreationParams&& appCreationParams)
        : m_platform_creation_params(rsl::move(appCreationParams.platform_params))
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
        event_system::subscribe(event_system::EventType::QuitApp, [this](const event_system::Event& /*event*/) { m_app_instance->quit(); });

        return m_on_initialize();
      }

      void update()
      {
        input::update();

        m_on_update();
      }

      void shutdown()
      {
        m_on_shutdown();
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
      , m_internal_obj()
      , m_internal(nullptr)
    {
      static_assert(s_internal_size == sizeof(ConsoleApplication::Internal), "size of internal structure should match size of internal implementation");
      static_assert(s_internal_alignment == alignof(ConsoleApplication::Internal), "alignment of internal structure should match alignment of internal implementation");

      m_internal = m_internal_obj.get<ConsoleApplication::Internal>();
      new (m_internal) (ConsoleApplication::Internal)(this, rsl::move(appParams));
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

  }
}
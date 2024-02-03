#include "rex_engine/app/core_application.h"

#include "rex_engine/diagnostics/debug.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/engine/engine_params.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_std/bonus/utility.h"

#include <cstdlib>

namespace rex
{

  namespace globals
  {
    FrameInfo g_frame_info; // NOLINT(fuchsia-statically-constructed-objects, cppcoreguidelines-avoid-non-const-global-variables)

    //-------------------------------------------------------------------------
    const FrameInfo& frame_info()
    {
      return g_frame_info;
    }
  } // namespace globals

  //-------------------------------------------------------------------------
  CoreApplication::CoreApplication(const EngineParams& engineParams)
      : m_app_state(ApplicationState::Created)
  {
    mem_tracker().initialize(engineParams.max_memory);
  }
  //-------------------------------------------------------------------------
  CoreApplication::~CoreApplication() = default;

  //--------------------------------------------------------------------------------------------
  s32 CoreApplication::run()
  {
    // calls the client shutdown count first, then shuts down the gui application systems
    const rsl::scopeguard shutdown_scopeguard([&]() { shutdown(); });

    // this calls our internal init code, to initialize the gui application
    // afterwards it calls into client code and initializes the code there
    // calling the initialize function provided earlier in the EngineParams
    if(initialize() == false) // NOLINT(readability-simplify-boolean-expr)
    {
      REX_ERROR(LogEngine, "Application initialization failed");
      return EXIT_FAILURE;
    }

    // Log memory usage after initialization has finished
    output_debug_string("Memory usage at after initialization");
    log_mem_usage();

    // calls into gui application update code
    // then calls into the client update code provided by the EngineParams before
    loop();

    // shutdown is automatically called from the scopeguard

    return EXIT_SUCCESS;
  }

  //-------------------------------------------------------------------------
  void CoreApplication::pause()
  {
    m_app_state.add_state(ApplicationState::Paused);
  }

  //-------------------------------------------------------------------------
  void CoreApplication::resume()
  {
    m_app_state.remove_state(ApplicationState::Paused);
  }

  //--------------------------------------------------------------------------------------------
  void CoreApplication::quit()
  {
    mark_for_destroy();
  }

  //-------------------------------------------------------------------------
  bool CoreApplication::is_paused() const
  {
    return m_app_state.has_state(ApplicationState::Paused);
  }

  //--------------------------------------------------------------------------------------------
  bool CoreApplication::is_running() const
  {
    return m_app_state.has_state(ApplicationState::Running) && !m_app_state.has_state(ApplicationState::MarkedForDestroy);
  }

  //--------------------------------------------------------------------------------------------
  bool CoreApplication::initialize()
  {
    m_app_state.change_state(ApplicationState::Initializing);

    // load the settings of the engine as early as possible
    // however it does have a few dependencies that need to be set up first
    // - commandline needs to be initialized
    // - vfs needs to be initialized

    globals::g_frame_info.update();
    return platform_init();
  }
  //--------------------------------------------------------------------------------------------
  void CoreApplication::update()
  {
    globals::g_frame_info.update();
    platform_update();
  }
  //--------------------------------------------------------------------------------------------
  void CoreApplication::shutdown()
  {
    platform_shutdown();

    // nothing to implement
  }
  //--------------------------------------------------------------------------------------------
  void CoreApplication::mark_for_destroy()
  {
    m_app_state.add_state(ApplicationState::MarkedForDestroy);
  }
  //--------------------------------------------------------------------------------------------
  void CoreApplication::loop()
  {
    m_app_state.change_state(ApplicationState::Running);

    while(is_running())
    {
      update();

      if(m_app_state.has_state(ApplicationState::MarkedForDestroy))
      {
        m_app_state.change_state(ApplicationState::ShuttingDown);
      }
    }
  }

} // namespace rex
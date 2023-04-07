#include "rex_engine/core_application.h"

#include "rex_engine/diagnostics/legacy/logging.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_std/assert.h"
#include "rex_std/bonus/utility/scopeguard.h"
#include "rex_std/chrono.h"
#include "rex_std/functional.h"
#include "rex_std/math.h"
#include "rex_std/memory.h"
#include "rex_std_extra/memory/memory_size.h"

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
  CoreApplication::CoreApplication(const EngineParams& engineParams, const CommandLineArguments& /*cmdArgs*/)
      : m_is_running(false)
      , m_is_marked_for_destroy(false)
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
      REX_ERROR("Application initialization failed");
      return EXIT_FAILURE;
    }

    // calls into gui application update code
    // then calls into the client update code provided by the EngineParams before
    loop();

    // shutdown is automatically called from the scopeguard

    return EXIT_SUCCESS;
  }

  //--------------------------------------------------------------------------------------------
  void CoreApplication::quit()
  {
    mark_for_destroy();
  }

  //--------------------------------------------------------------------------------------------
  bool CoreApplication::is_running() const
  {
    return m_is_running && !m_is_marked_for_destroy;
  }

  //--------------------------------------------------------------------------------------------
  bool CoreApplication::initialize()
  {
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
    m_is_marked_for_destroy = true;
  }
  //--------------------------------------------------------------------------------------------
  void CoreApplication::loop()
  {
    m_is_running = true;

    while(is_running())
    {
      update();

      m_is_running = !m_is_marked_for_destroy;
    }
  }

} // namespace rex
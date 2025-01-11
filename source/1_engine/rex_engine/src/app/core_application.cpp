#include "rex_engine/app/core_application.h"

#include "rex_engine/diagnostics/debug.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/engine/engine_params.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/settings/settings.h"
#include "rex_std/bonus/utility.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/profiling/scoped_timer.h"
#include "rex_engine/cmdline/cmdline.h"

#include "rex_engine/engine/mutable_globals.h"

#include <cstdlib>

namespace rex
{
  DEFINE_LOG_CATEGORY(LogCoreApp);

  //-------------------------------------------------------------------------
  CoreApplication::CoreApplication(const EngineParams& engineParams)
		: m_app_state(ApplicationState::Created)
    , m_app_name(engineParams.app_name)
    , m_exit_code(0)
  {
  }
  //-------------------------------------------------------------------------
  CoreApplication::~CoreApplication()
  {
    vfs::shutdown();

    cmdline::shutdown();
  }

  //--------------------------------------------------------------------------------------------
  s32 CoreApplication::run()
  {
    // calls the client shutdown count first, then shuts down the gui application systems
    const rsl::scopeguard shutdown_scopeguard([&]() { shutdown(); });

    // Log memory usage before initialization has started
    output_debug_string("Memory usage before initialization");
    debug_log_mem_usage();

    // this calls our internal init code, to initialize the gui application
    // afterwards it calls into client code and initializes the code there
    // calling the initialize function provided earlier in the EngineParams
    if(initialize() == false) // NOLINT(readability-simplify-boolean-expr)
    {
      REX_ERROR(LogEngine, "Application initialization failed");
      return EXIT_FAILURE;
    }

    // Log memory usage after initialization has finished
    output_debug_string("Memory usage after initialization");
    debug_log_mem_usage();

    // calls into gui application update code
    // then calls into the client update code provided by the EngineParams before
    loop();

    // shutdown is automatically called from the scopeguard

    return m_exit_code;
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
  void CoreApplication::quit(rsl::string_view reason, s32 exitCode)
  {
    REX_INFO(LogCoreApp, "Quitting app. Reason: {}", reason);

    mark_for_destroy(exitCode);
  }

  //-------------------------------------------------------------------------
  bool CoreApplication::is_initializing() const
  {
    return m_app_state.has_state(ApplicationState::Initializing);
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
  bool CoreApplication::is_marked_for_destroy() const
  {
    return m_app_state.has_state(ApplicationState::MarkedForDestroy);
  }

  //--------------------------------------------------------------------------------------------
  bool CoreApplication::is_shutting_down() const
  {
    return m_app_state.has_state(ApplicationState::ShuttingDown);
  }
  //--------------------------------------------------------------------------------------------
  rsl::string_view CoreApplication::app_name() const
  {
    return m_app_name;
  }

  //--------------------------------------------------------------------------------------------
  bool CoreApplication::initialize()
  {
    m_app_state.change_state(ApplicationState::Initializing);

    init_globals();

    // Loads the mounts of the engine
    // this will make it easier to access files under these paths
    // in the future
    mount_engine_paths();

    // load the settings of the engine as early as possible
    // however it does have a few dependencies that need to be set up first
    // - commandline needs to be initialized
    // - vfs needs to be initialized
    load_settings();

    rex::mut_globals().frame_info.update();

    REX_INFO(LogCoreApp, "core engine systems initialized");
    const bool res = platform_init();

    // Some settings get overriden in the editor and the project
    // so we can only use those settings after they've been loaded.
    // the max allowed memory usage is one of those examples

    // Settings are loaded now, we can initialize all the sub systems with settings loaded from them
    const rsl::memory_size max_mem_budget = rsl::memory_size::from_mib(settings::get_int("max_memory_mib"));
    mem_tracker().initialize(max_mem_budget);

    return res;
  }
  //--------------------------------------------------------------------------------------------
  void CoreApplication::update()
  {
    rex::mut_globals().frame_info.update();
    rex::mut_globals().single_frame_allocator->adv_frame();

    platform_update();
  }
  //--------------------------------------------------------------------------------------------
  void CoreApplication::shutdown()
  {
    platform_shutdown();

    end_profiling_session();
  }
  //--------------------------------------------------------------------------------------------
  void CoreApplication::mark_for_destroy(s32 exitCode)
  {
    m_app_state.remove_state(ApplicationState::Paused); // It's possible the application is currently paused, in which case, remove that flag
    m_app_state.add_state(ApplicationState::MarkedForDestroy);
    m_exit_code = exitCode;
  }
  //--------------------------------------------------------------------------------------------
  void CoreApplication::loop()
  {
    m_app_state.change_state(ApplicationState::Running);

    while(is_running())
    {
      REX_PROFILE_FUNCTION();

      update();

      if(m_app_state.has_state(ApplicationState::MarkedForDestroy))
      {
        m_app_state.change_state(ApplicationState::ShuttingDown);
      }
    }
  }

  //--------------------------------------------------------------------------------------------
  void CoreApplication::mount_engine_paths() // NOLINT(readability-convert-member-functions-to-static)
  {
    vfs::mount(MountingPoint::EngineRoot, vfs::engine_root());

    vfs::mount(MountingPoint::EngineSettings, path::join(vfs::engine_root(), "settings"));
    vfs::mount(MountingPoint::EngineMaterials, path::join(vfs::engine_root(), "materials"));
    vfs::mount(MountingPoint::EngineShaders, path::join(vfs::engine_root(), "shaders"));
  }

  //--------------------------------------------------------------------------------------------
  void CoreApplication::load_settings() // NOLINT(readability-convert-member-functions-to-static)
  {
    // Load the engine settings.
    // They can always be overridden in a project
    // but the engine loads the default settings

    // get the default settings of the engine and load them into memory
    const rsl::vector<rsl::string> files = directory::list_files(vfs::mount_path(MountingPoint::EngineSettings));

    for(const rsl::string_view file: files)
    {
      settings::load(file);
    }
  }

  //--------------------------------------------------------------------------------------------
  void CoreApplication::init_globals()
  {
    mut_globals().single_frame_allocator = rsl::make_unique<FrameBasedAllocator>(settings::get_int("SingleFrameAllocatorSize", 1_mib), 1);
  }

} // namespace rex
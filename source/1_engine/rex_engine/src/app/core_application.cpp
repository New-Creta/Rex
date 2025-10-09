#include "rex_engine/app/core_application.h"

#include "rex_engine/diagnostics/debug.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/engine/asset_db.h"
#include "rex_engine/engine/engine_params.h"
#include "rex_engine/engine/module_manager.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/settings/settings.h"
#include "rex_engine/system/process.h"
#include "rex_engine/event_system/event_system.h"
#include "rex_std/bonus/utility.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/profiling/timer.h"
#include "rex_engine/profiling/profiling_session.h"
#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/filesystem/native_filesystem.h"
#include "rex_engine/threading/thread_pool.h"

#include "rex_engine/assets/map.h"
#include "rex_engine/assets/blockset.h"
#include "rex_engine/assets/texture_asset.h"
#include "rex_engine/assets/flipbook.h"
#include "rex_engine/assets/input_mapping.h"

#include "rex_engine/serialization/text_loaders/map_loader_json.h"
#include "rex_engine/serialization/text_loaders/tileset_loader_json.h"
#include "rex_engine/serialization/text_loaders/blockset_loader_json.h"
#include "rex_engine/serialization/text_loaders/flipbook_loader_json.h"
#include "rex_engine/serialization/text_loaders/input_mapping_loader_json.h"
#include "rex_engine/serialization/binary_loaders/texture_loader.h"

#include "rex_std/internal/exception/exit.h"

#include <cstdlib>

namespace rex
{
  DEFINE_LOG_CATEGORY(LogCoreApp);

  //-------------------------------------------------------------------------
  CoreApplication::CoreApplication(const EngineInitParams& engineParams)
		: m_app_state(ApplicationState::Created)
    , m_app_name(engineParams.app_name)
    , m_exit_code(0)
  {
  }
  //-------------------------------------------------------------------------
  CoreApplication::~CoreApplication()
  {
  }

  //--------------------------------------------------------------------------------------------
  s32 CoreApplication::run()
  {
    // calls the client shutdown count first, then shuts down the gui application systems
    const rsl::scopeguard shutdown_scopeguard([&]() { shutdown(); });

    // Log memory usage before initialization has started
    output_debug_string("Memory usage before initialization");
    debug_log_mem_usage();

    // Make sure to set the exit code as it gets logged in shutdown
    // if any of the below fails without setting the exit code
    // we at least have set it here
    m_exit_code = EXIT_FAILURE;

    // this calls our internal init code, to initialize the gui application
    // afterwards it calls into client code and initializes the code there
    // calling the initialize function provided earlier in the EngineInitParams
    if(initialize() == false) // NOLINT(readability-simplify-boolean-expr)
    {
      REX_ERROR(LogEngine, "Application initialization failed");

      return m_exit_code;
    }

    // Log memory usage after initialization has finished
    output_debug_string("Memory usage after initialization");
    debug_log_mem_usage();

    // calls into gui application update code
    // then calls into the client update code provided by the EngineInitParams before
    m_exit_code = EXIT_SUCCESS;
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
    REX_UNUSED_PARAM(reason);

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

    engine::instance()->advance_frame();

    REX_INFO(LogCoreApp, "core engine systems initialized");
    const bool res = platform_init();

    // Some settings get overriden in the editor and the project
    // so we can only use those settings after they've been loaded.
    // the max allowed memory usage is one of those examples

    // Settings are loaded now, we can initialize all the sub systems with settings loaded from them
    const rsl::memory_size max_mem_budget = rsl::memory_size::from_mib(settings::instance()->get_int("max_memory_mib"));
    mem_tracker().initialize(max_mem_budget);

    return res;
  }
  //--------------------------------------------------------------------------------------------
  void CoreApplication::update()
  {
    engine::instance()->advance_frame();

    platform_update();
  }
  //--------------------------------------------------------------------------------------------
  void CoreApplication::shutdown()
  {
    REX_INFO(LogCoreApp, "Shutting down application..");

    platform_shutdown();

    REX_INFO(LogEngine, "Application shutdown with result: {0}", m_exit_code);

    shutdown_globals();
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
    vfs::instance()->mount(MountingPoint::EngineRoot, engine::instance()->engine_root());

    vfs::instance()->mount(MountingPoint::EngineSettings, path::join(engine::instance()->engine_root(), "settings"));
    vfs::instance()->mount(MountingPoint::EngineMaterials, path::join(engine::instance()->engine_root(), "materials"));
    vfs::instance()->mount(MountingPoint::EngineShaders, path::join(engine::instance()->engine_root(), "shaders"));

    vfs::instance()->mount(rex::MountingPoint::Logs, path::join(engine::instance()->current_session_root(), "logs"));
  }

  //--------------------------------------------------------------------------------------------
  void CoreApplication::load_settings() // NOLINT(readability-convert-member-functions-to-static)
  {
    // Load the engine settings.
    // They can always be overridden in a project
    // but the engine loads the default settings

    settings::init(globals::make_unique<SettingsManager>());

    // get the default settings of the engine and load them into memory
    const rsl::vector<rsl::string> files = directory::list_files(vfs::instance()->mount_path(MountingPoint::EngineSettings));

    scratch_string fullpath;
    for(const rsl::string_view file: files)
    {
      fullpath.clear();
      fullpath = path::join(vfs::instance()->mount_path(MountingPoint::EngineSettings), file);
      REX_DEBUG(LogCoreApp, "Loading settings file: {}", fullpath);
      settings::instance()->load(fullpath);
    }
  }

  //--------------------------------------------------------------------------------------------
  BootSettings CoreApplication::load_boot_settings()
  {
    BootSettings boot_settings{};

    scratch_string abs_boot_ini_path = vfs::instance()->abs_path("rex/settings/boot.ini");
    if (vfs::instance()->exists(abs_boot_ini_path))
    {
      boot_settings = parse_boot_settings(abs_boot_ini_path);
    }

    return boot_settings;
  }

  //--------------------------------------------------------------------------------------------
  void CoreApplication::init_engine_globals(const BootSettings& bootSettings)
  {
    // Read the settings of the file
    REX_ASSERT_X(bootSettings.single_frame_heap_size > 0, "Single frame heap setting indicates 0 size. The setting is either missing or 0. Please add a setting to \"heaps\" with name \"single_frame_heap_size\" in memory_settings.ini");
    REX_ASSERT_X(bootSettings.scratch_heap_size > 0, "Scratch heap setting indicates 0 size. The setting is either missing or 0. Please add a setting to \"heaps\" with name \"scratch_heap_size\" in memory_settings.ini");

    // Initialize the global heaps and its allocators using the settings loaded from disk
    auto scratch_alloc = rsl::make_unique<TCircularAllocator<GlobalAllocator>>(bootSettings.scratch_heap_size);
    auto single_frame_alloc = rsl::make_unique<TStackAllocator<GlobalAllocator>>(bootSettings.single_frame_heap_size);

    engine::init(globals::make_unique<EngineGlobals>(rsl::move(scratch_alloc), rsl::move(single_frame_alloc)));
  }

  //--------------------------------------------------------------------------------------------
  void CoreApplication::init_cmdline()
  {
    scratch_string cmd_args_path;
    
    // Load the commandline arguments of this module
    path::join_to(cmd_args_path, module_manager::instance()->current()->data_path(), "cmdline_args.json");
    if (vfs::instance()->exists(cmd_args_path))
    {
      REX_DEBUG(LogCoreApp, "Loading commandline arguments file: {}", cmd_args_path);
      cmdline::instance()->load_arguments_from_file(cmd_args_path, module_manager::instance()->current()->name());
    }

    // Load the commandline arguments of the dependencies
    for (Module* dependency : module_manager::instance()->current()->dependencies())
    {
      cmd_args_path.clear();
      path::join_to(cmd_args_path, dependency->data_path(), "cmdline_args.json");
      if (vfs::instance()->exists(cmd_args_path))
      {
        REX_DEBUG(LogCoreApp, "Loading commandline arguments file: {}", cmd_args_path);
        cmdline::instance()->load_arguments_from_file(cmd_args_path, dependency->name());
      }
    }

    cmdline::instance()->post_init();

    // if a user wants to know the arguments for the executable, we want to perform as minimal setup as possible.
    // we just initialize the commandline, print what's possible and exit the program
    if (cmdline::instance()->get_argument("help"))
    {
      cmdline::instance()->help();
      rsl::exit(0);
    }
  }

  //--------------------------------------------------------------------------------------------
  void CoreApplication::init_thread_pool()
  {
    thread_pool::init(globals::make_unique<ThreadPool>());
  }

  //--------------------------------------------------------------------------------------------
  void CoreApplication::init_asset_db()
  {
    asset_db::init(globals::make_unique<AssetDb>());

    asset_db::instance()->add_loader<Map>(rsl::make_unique<MapLoaderJson>());
    asset_db::instance()->add_loader<Blockset>(rsl::make_unique<BlocksetLoaderJson>());
    asset_db::instance()->add_loader<TilesetAsset>(rsl::make_unique<TilesetLoaderJson>());
    asset_db::instance()->add_loader<TextureAsset>(rsl::make_unique<TextureLoader>());
    asset_db::instance()->add_loader<Flipbook>(rsl::make_unique<FlipbookLoaderJson>());
    asset_db::instance()->add_loader<InputMapping>(rsl::make_unique<InputMappingLoaderJson>());
  }

  //--------------------------------------------------------------------------------------------
  void CoreApplication::init_globals()
  {
    REX_DEBUG(LogCoreApp, "Initializing virtual filesystem");
    rsl::string_view root = cmdline::instance()->get_argument("root").value_or(path::cwd());
    vfs::init(globals::make_unique<NativeFileSystem>(root));

    REX_DEBUG(LogCoreApp, "Initializing module manager");
    module_manager::init(globals::make_unique<ModuleManager>());

    REX_DEBUG(LogCoreApp, "Loading boot settings");
    BootSettings boot_settings = load_boot_settings();

    REX_DEBUG(LogCoreApp, "Initializing engine globals");
    init_engine_globals(boot_settings);

    REX_DEBUG(LogCoreApp, "Initializing commandline arguments");
    init_cmdline();

    REX_DEBUG(LogCoreApp, "Initializing thread pool");
    init_thread_pool();

    // Loads the mounts of the engine
    // this will make it easier to access files under these paths
    // in the future
    REX_DEBUG(LogCoreApp, "Mounting engine paths");
    mount_engine_paths();

    // load the settings of the engine as early as possible
    // however it does have a few dependencies that need to be set up first
    // - commandline needs to be initialized
    // - vfs needs to be initialized
    REX_DEBUG(LogCoreApp, "Loading all settings");
    load_settings();

    REX_DEBUG(LogCoreApp, "Initializing event system");
    event_system::init(globals::make_unique<EventSystem>());

    REX_DEBUG(LogCoreApp, "Initializing profiling system");
    profiling_session::init(globals::make_unique<ProfilingSession>());

    REX_DEBUG(LogCoreApp, "Initializing asset database");
    init_asset_db();
  }

  //--------------------------------------------------------------------------------------------
  BootSettings CoreApplication::parse_boot_settings(rsl::string_view bootSettingsPath)
  {
    REX_ASSERT_X(vfs::instance()->exists(bootSettingsPath), "boot settings path for parsing doesn't exist. {}", bootSettingsPath);
    ini::Ini boot_settings_ini = ini::read_from_file(bootSettingsPath);

    BootSettings boot_settings{};

    boot_settings.single_frame_heap_size = rsl::stoi(boot_settings_ini.get("heaps", "single_frame_heap_size", "<invalid int>")).value_or(boot_settings.single_frame_heap_size);
    boot_settings.scratch_heap_size = rsl::stoi(boot_settings_ini.get("heaps", "scratch_heap_size", "<invalid int>")).value_or(boot_settings.scratch_heap_size);

    return boot_settings;
  }

  //--------------------------------------------------------------------------------------------
  void CoreApplication::shutdown_globals()
  {
    REX_INFO(LogCoreApp, "Shutting down globals");

    asset_db::shutdown();
    profiling_session::shutdown();
    event_system::shutdown();
    settings::shutdown();
    module_manager::shutdown();
    vfs::shutdown();
    thread_pool::shutdown();
    cmdline::shutdown();
    engine::shutdown();

    globals::disable_global_destruction();
  }

} // namespace rex
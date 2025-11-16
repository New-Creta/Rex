#pragma once

#include "rex_engine/app/core_application_state.h"
#include "rex_engine/engine/state_controller.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/input/input_state.h"
#include "rex_engine/settings/boot_settings.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_engine/text_processing/ini.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/functional.h"
#include "rex_std/limits.h"

namespace rex
{
  class FrameInfo;
  struct EngineInitParams;

  class CoreApplication
  {
  public:
    explicit CoreApplication(const EngineInitParams& engineParams);
    CoreApplication(const CoreApplication&) = delete;
    CoreApplication(CoreApplication&&)      = delete;
    virtual ~CoreApplication();

    CoreApplication& operator=(const CoreApplication&) = delete;
    CoreApplication& operator=(CoreApplication&&)      = delete;

    // Running the application is considered to be the entry point of the application
    // it will initialize, update and shutdown the application
    // returning an error code or 0 on success
    s32 run();

    // pausing/resuming the app will set or remove the pause state flag
    // this has no effect on the core loop of the app
    // but the user can query this state and enable/disable ticks or systems
    void pause();
    void resume();

    // quiting doesn't quit immediately but marks the application for quiting
    // on the start of the next tick, the program will shutdown
    void quit(rsl::string_view reason = "", s32 exitCode = 0);

    // useful flag detection functions
    bool is_initializing() const;
    bool is_paused() const;
    bool is_running() const;
    bool is_marked_for_destroy() const;
    bool is_shutting_down() const;

    // return the name of the application that was specified on construction
    rsl::string_view app_name() const;

  protected:
    // init, update and shutdown implementation
    // for platform specific layer
    virtual bool platform_init()     = 0;
    virtual void platform_update()   = 0;
    virtual void platform_shutdown() = 0;

  private:
    // core functions
    bool initialize();
    void update();
    void shutdown();
    void mark_for_destroy(s32 exitCode);
    void loop();

    // Initialization
    // Initialize global systems (eg. filesystem, engine globals, thread pool, ..)
    void init_globals();
    // The boot settings are one the first settings loaded by the engine
    // They are used to initialize the engine globals
    BootSettings load_boot_settings();
    // Initialize the engine globals using the boot settings
    void init_engine_globals(const BootSettings& bootSettings);
    // load the commandline options of the application module and its dependencies
    void init_cmdline();
    // mount default engine paths onto the filesystem
    void mount_engine_paths();
    // load all engine settings into memory
    void load_settings();
    void init_thread_pool();
    // initialize asset db and adding its serializers
    void init_asset_db();

    // load the boot settings from disk, parse and initialize them
    BootSettings parse_boot_settings(rsl::string_view bootSettingsPath);

    // Shutdown
    // destroy all globals in reverse order of their initialization
    void shutdown_globals();

  private:
    // The input state holds which keys are up or down
    // We need to use our own state mechanism and cannot forward them directly from the OS
    // as it's possible the user ignores key input between frames, at a different rate than
    // the OS sending the event a key is down
    // Example:
    // users performs an animation on key entry and ignores input while animation is playing
    // if the animation lasts 8 frames, and on frame 7 a key is pressed and held until frame 10
    // but the OS only sends key down events every 20 frames
    // you will have missed a key event as it'd be ignored
    // storing the state ourselves and resending the event until a key up event comes in solves this problem
    rsl::unique_ptr<InputState> m_input_state;

    // The app state is simply a bit mask.
    // this will do for now but if this won't be enough we may make this more complex
    StateController<ApplicationState> m_app_state;

    // The app name given by the user on construction
    rsl::string m_app_name;

    // The exit code of the app.
    // this is held as a member so it can be set from anywhere
    // the shutdown logic will use this value to return from the "run" function
    s32 m_exit_code;
  };
} // namespace rex
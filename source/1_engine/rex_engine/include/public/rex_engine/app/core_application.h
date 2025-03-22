#pragma once

#include "rex_engine/app/core_application_state.h"
#include "rex_engine/engine/state_controller.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_engine/text_processing/ini.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/functional.h"
#include "rex_std/limits.h"

namespace rex
{
  class FrameInfo;
  struct EngineParams;

  class CoreApplication
  {
  public:
    explicit CoreApplication(const EngineParams& engineParams);
    CoreApplication(const CoreApplication&) = delete;
    CoreApplication(CoreApplication&&)      = delete;
    virtual ~CoreApplication();

    CoreApplication& operator=(const CoreApplication&) = delete;
    CoreApplication& operator=(CoreApplication&&)      = delete;

    s32 run();

    void pause();
    void resume();
    void quit(rsl::string_view reason = "", s32 exitCode = 0);

    bool is_initializing() const;
    bool is_paused() const;
    bool is_running() const;
    bool is_marked_for_destroy() const;
    bool is_shutting_down() const;
    rsl::string_view app_name() const;

  protected:
    virtual bool platform_init()     = 0;
    virtual void platform_update()   = 0;
    virtual void platform_shutdown() = 0;

  private:
    bool initialize();
    void update();
    void shutdown();
    void mark_for_destroy(s32 exitCode);
    void loop();

    // Initialization
    void mount_engine_paths();
    void load_settings();
    void init_boot_settings();
    void init_allocators(const rex::ini::Ini& bootSettings);
    void init_globals();

  private:
    StateController<ApplicationState> m_app_state;
    rsl::string m_app_name;
    s32 m_exit_code;
  };
} // namespace rex
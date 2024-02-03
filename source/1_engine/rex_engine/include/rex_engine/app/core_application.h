#pragma once

#include "rex_engine/app/core_application_state.h"
#include "rex_engine/engine/state_controller.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/functional.h"
#include "rex_std/limits.h"

namespace rex
{
  class FrameInfo;
  struct EngineParams;

  // to make it accessible throughout the code
  // client API can call this function to access the frame info
  namespace globals
  {
    const FrameInfo& frame_info();
  } // namespace globals

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
    void quit();

    bool is_paused() const;
    bool is_running() const;

  protected:
    virtual bool platform_init()     = 0;
    virtual void platform_update()   = 0;
    virtual void platform_shutdown() = 0;

  private:
    bool initialize();
    void update();
    void shutdown();
    void mark_for_destroy();
    void loop();

    void load_settings();

  private:
    StateController<ApplicationState> m_app_state;
  };
} // namespace rex
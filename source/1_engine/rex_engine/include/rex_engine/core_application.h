#pragma once

#include "rex_engine/cmd_line_args.h"
#include "rex_engine/core_application_state.h"
#include "rex_engine/state_controller.h"
#include "rex_engine/types.h"
#include "rex_std/functional.h"
#include "rex_std/limits.h"
#include "rex_std/memory.h"
#include "rex_std_extra/memory/memory_size.h"

namespace rex
{
  struct EngineParams
  {
    using init_func     = rsl::function<bool()>;
    using update_func   = rsl::function<void()>;
    using shutdown_func = rsl::function<void()>;

    // how much memory is the entire app allowed to use.
    // by default, there's no limit on this.
    rsl::memory_size max_memory     = rsl::memory_size((rsl::numeric_limits<s64>::max)());
    init_func app_init_func         = nullptr;
    update_func app_update_func     = nullptr;
    shutdown_func app_shutdown_func = nullptr;
  };

  class FrameInfo;

  // to make it accessible throughout the code
  // client API can call this function to access the frame info
  namespace globals
  {
    const FrameInfo& frame_info();
  } // namespace globals

  class CoreApplication
  {
  public:
    CoreApplication(const EngineParams& engineParams, const CommandLineArguments& cmdArgs);
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

  private:
    StateController<ApplicationState> m_app_state;
  };
} // namespace rex
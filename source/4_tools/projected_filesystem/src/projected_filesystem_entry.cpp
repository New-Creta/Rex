#include "rex_engine/cmdline.h"

#include "rex_engine/entrypoint.h"
#include "rex_engine/event_system.h"

#include "rex_windows/filesystem/drive_handle.h"
#include "rex_windows/filesystem/usn_journal.h"
#include "rex_windows/platform_creation_params.h"
#include "rex_windows/input/input.h"

#include "projected_filesystem/projected_filesystem.h"

#include "rex_std/thread.h"
#include "rex_std/chrono.h"

namespace proj_fs
{
  ProjectedFilesystem* projected_fs()
  {
    static auto fs = rsl::make_unique<ProjectedFilesystem>("D:\\ProjFS\\branches");;
    return fs.get();
  }

  bool initialize()
  {
    projected_fs();

    return true;
  }
  void update()
  {
    while (true)
    {
      rex::input::internal::update();

      if (rex::input::is_key_pressed('Q'))
      {
        break;
      }

      using namespace rsl::chrono_literals;
      rsl::this_thread::sleep_for(1ms);
    }

    rex::event_system::Event ev{};
    ev.type = rex::event_system::EventType::QuitApp;
    rex::event_system::fire_event(ev);
  }
  void shutdown()
  {
  }
}

namespace rex
{
  ApplicationCreationParams app_entry(PlatformCreationParams&& platformParams)
  {
    ApplicationCreationParams app_params(rsl::move(platformParams));

    app_params.engine_params.max_memory = 256_kb;
    app_params.engine_params.app_init_func = proj_fs::initialize;
    app_params.engine_params.app_update_func = proj_fs::update;
    app_params.engine_params.app_shutdown_func = proj_fs::shutdown;

    app_params.create_window = false;

    return app_params;
  }
}
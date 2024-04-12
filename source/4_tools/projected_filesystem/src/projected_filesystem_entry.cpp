#include "rex_engine/cmdline/cmdline.h"

#include "rex_engine/engine/entrypoint.h"
#include "rex_engine/event_system/event_system.h"

#include "rex_windows/filesystem/drive_handle.h"
#include "rex_windows/filesystem/usn_journal.h"
#include "rex_windows/engine/platform_creation_params.h"
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
    rex::event_system::enqueue_event(rex::event_system::EventType::QuitApp);
  }
  void shutdown()
  {
  }
}

namespace rex
{
  ApplicationCreationParams app_entry(PlatformCreationParams& platformParams)
  {
    ApplicationCreationParams app_params(platformParams);

    app_params.engine_params.app_init_func = proj_fs::initialize;
    app_params.engine_params.app_update_func = proj_fs::update;
    app_params.engine_params.app_shutdown_func = proj_fs::shutdown;

    app_params.create_window = false;

    return app_params;
  }
}
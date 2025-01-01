#include "rex_engine/cmdline/cmdline.h"

#include "rex_engine/engine/entrypoint.h"
#include "rex_windows/engine/platform_creation_params.h"
#include "rex_engine/event_system/event_system.h"

#include "rex_windows/filesystem/drive_handle.h"
#include "rex_windows/filesystem/usn_journal.h"

#include "rex_std/iostream.h"

namespace ntfs_journal
{
  bool initialize(const rex::ApplicationCreationParams& /*appCreationParams*/)
  {
    return true;
  }
  void update()
  {
    rex::win32::DriveHandle drive(rex::cmdline::get_argument("Drive").value_or("P")[0]);
    rex::win32::UsnJournal journal(rsl::move(drive), rex::cmdline::get_argument("Output").value_or("last_usn.txt"));
    rsl::vector<rsl::string> files = journal.read();

    // we print using cout directly to minimize the extra messages logged to screen
    // This allows a script to run this app, parse the output and only parse the output
    // which would be the files that changed since the last time it ran
    for (rsl::string_view file : files)
    {
      rsl::cout << file << "\n";
    }
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

    app_params.engine_params.app_init_func = ntfs_journal::initialize;
    app_params.engine_params.app_update_func = ntfs_journal::update;
    app_params.engine_params.app_shutdown_func = ntfs_journal::shutdown;

    app_params.is_gui_app = false;

    return app_params;
  }
}
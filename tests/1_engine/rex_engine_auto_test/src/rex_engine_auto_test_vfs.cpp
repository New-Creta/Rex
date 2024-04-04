#include "rex_engine_auto_test/rex_engine_auto_test_vfs.h"
#include "rex_engine/event_system/event_system.h"

#include "rex_engine/engine/engine_params.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/filesystem/file.h"

namespace rex
{
  namespace auto_tests
  {
    DEFINE_LOG_CATEGORY(LogVfsAutoTest);

    bool initialize()
    {
      bool result = true;

      // vfs is initialized by now, so we can do our testing here

      rsl::string_view expected_content = "this is a test file with some test content";
      memory::Blob content_blob = vfs::read_file(path::join(vfs::root(), "rexengineautotest", "test_file.txt"));
      rsl::string_view file_content((const char8*)content_blob.data(), (s32)content_blob.size()-1);
      if (expected_content != file_content)
      {
        REX_ERROR(LogVfsAutoTest, "\"test_file.txt\" does not have the expected content: \"{}\" (with size {}).", expected_content, expected_content.size());
        REX_ERROR(LogVfsAutoTest, "Instead the retrieved content is: \"{}\" (with size {}).", file_content, file_content.size());
        result = false;
      }

      if (result)
      {
        REX_INFO(LogVfsAutoTest, "All tests succeeded");
      }

      return result;
    }
    void update()
    {
      // We don't need to do any further testing, so as soon as initialization succeeds, we can exit here
      rex::event_system::enqueue_event(rex::event_system::EventType::QuitApp);
    }
    void shutdown()
    {
    }

    rex::ApplicationCreationParams vfs_test_entry(rex::PlatformCreationParams& platformParams)
    {
      rex::ApplicationCreationParams app_params(platformParams);

      app_params.gui_params.window_title = "VFS Test";
      app_params.create_window = false;

      app_params.engine_params.app_init_func = auto_tests::initialize;
      app_params.engine_params.app_update_func = auto_tests::update;
      app_params.engine_params.app_shutdown_func = auto_tests::shutdown;

      return app_params;
    }

  }
}

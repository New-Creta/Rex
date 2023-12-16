#include "rex_auto_test/auto_test.h"

#include "rex_engine/engine_params.h"

namespace rex
{
  namespace auto_test
  {
    AutoTest::AutoTest(rsl::string_view cmdline, auto_test_entry entryFunc)
      : m_cmd_line(cmdline)
      , m_entry_func(entryFunc)
    {}

    bool AutoTest::is_enabled(rsl::string_view cmdline) const
    {
      return rsl::strincmp(cmdline.data(), m_cmd_line.data(), cmdline.length()) == 0;
    }

    ApplicationCreationParams AutoTest::launch(rex::PlatformCreationParams&& platformParams)
    {
      return m_entry_func(rsl::move(platformParams));
    }


    // back up function in case we fail to find an auto test
    bool bad_init()
    {
      return false;
    }

    rex::ApplicationCreationParams invalid_app_params(rex::PlatformCreationParams&& platformParams)
    {
      rex::ApplicationCreationParams app_params(&platformParams);

      app_params.engine_params.app_init_func = rex::auto_test::bad_init;

      return app_params;
    }
  }
}
#include "rex_auto_test/auto_test.h"

#include "rex_engine/engine/engine_params.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/cmdline/cmdline.h"

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

    ApplicationCreationParams AutoTest::launch(rex::PlatformCreationParams& platformParams)
    {
      return m_entry_func(platformParams);
    }


    // back up function in case we fail to find an auto test
    bool bad_init()
    {
      return false;
    }

    rex::ApplicationCreationParams invalid_app_params(rex::PlatformCreationParams&& platformParams)
    {
      rex::ApplicationCreationParams app_params(platformParams);

      app_params.engine_params.app_init_func = rex::auto_test::bad_init;

      return app_params;
    }

  }

  rex::ApplicationCreationParams app_entry(rex::PlatformCreationParams& platformParams)
  {
    rsl::optional<rsl::string_view> cmdline = rex::cmdline::get_argument("AutoTest");

    REX_ASSERT_X(cmdline.has_value(), "Auto test fired but no auto test specified on the commandline. Commandline: {}", rex::cmdline::get());

    rsl::vector<auto_test::AutoTest> auto_tests = auto_test::auto_test_entry();

    for (rex::auto_test::AutoTest& auto_test : auto_tests)
    {
      if (auto_test.is_enabled(cmdline.value()))
      {
        return auto_test.launch(rsl::move(platformParams));
      }
    }

    REX_ASSERT("No auto test found for {}", cmdline.value());

    return rex::auto_test::invalid_app_params(rsl::move(platformParams));
  }
}
#include "rex_engine/entrypoint.h"
#include "rex_engine/engine_params.h"
#include "rex_windows/gui_application.h"

#include "regina_test/regina_boot_test.h"
#include "regina_test/regina_vfs_test.h"
#include "rex_engine/cmdline.h"
#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  class AutoTest
  {
  public:
    using auto_test_entry = rex::ApplicationCreationParams(*)(rex::PlatformCreationParams&&);

    AutoTest(rsl::string_view cmdline, auto_test_entry entryFunc)
      : m_cmd_line(cmdline)
      , m_entry_func(entryFunc)
    {}

    bool is_enabled(rsl::string_view cmdline)
    {
      return rsl::strincmp(cmdline.data(), m_cmd_line.data(), cmdline.length()) == 0;
    }

    ApplicationCreationParams launch(rex::PlatformCreationParams&& platformParams)
    {
      return m_entry_func(rsl::move(platformParams));
    }

  private:
    rsl::string_view m_cmd_line;
    auto_test_entry m_entry_func;
  };

  rsl::array g_auto_tests =
  {
    AutoTest("Boot", regina_boot_test::boot_test_entry),
    AutoTest("Vfs", regina_vfs_test::vfs_test_entry)
  };

  // back up function in case we fail to find an auto test
  bool bad_init()
  {
    return false;
  }

  rex::ApplicationCreationParams invalid_app_params(rex::PlatformCreationParams&& platformParams)
  {
    rex::ApplicationCreationParams app_params(rsl::move(platformParams));

    app_params.engine_params.app_init_func = rex::bad_init;

    return app_params;
  }

  rex::ApplicationCreationParams app_entry(rex::PlatformCreationParams&& platformParams)
  {
    rsl::optional<rsl::string_view> cmdline = cmdline::get_argument("AutoTest");

    REX_ASSERT_X(cmdline.has_value(), "Auto test fired but no auto test specified on the commandline");

    for (AutoTest& auto_test : g_auto_tests)
    {
      if (auto_test.is_enabled(cmdline.value()))
      {
        return auto_test.launch(rsl::move(platformParams));
      }
    }

    REX_ASSERT("No auto test found for {}", cmdline.value());
      
    return invalid_app_params(rsl::move(platformParams));
  }
} // namespace rex
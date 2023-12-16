#include "rex_engine/entrypoint.h"
#include "rex_engine/engine_params.h"
#include "rex_windows/gui_application.h"

#include "regina_auto_test/regina_boot_test.h"
#include "rex_engine/cmdline.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_windows/platform_creation_params.h"

#include "rex_auto_test/auto_test.h"

namespace regina
{
  rsl::array g_auto_tests =
  {
    rex::auto_test::AutoTest("Boot", regina_auto_test::boot_test_entry)
  };

} // namespace regina

namespace rex
{
  rex::ApplicationCreationParams app_entry(rex::PlatformCreationParams&& platformParams)
  {
    rsl::optional<rsl::string_view> cmdline = rex::cmdline::get_argument("AutoTest");

    REX_ASSERT_X(cmdline.has_value(), "Auto test fired but no auto test specified on the commandline. Commandline: {}", rex::cmdline::get());

    for (rex::auto_test::AutoTest& auto_test : regina::g_auto_tests)
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
// Basic functionality to create auto tests
#include "rex_auto_test/auto_test.h"

// Entry point we need to define for the rex engine
#include "rex_engine/engine/entrypoint.h"

// Holds the definition of ApplicationCreationParams
#include "rex_engine/engine/engine_params.h"

// Includes to query and diagnose the auto test to run
#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/assert.h"

// Holds the definition of PlatformCreationParams
#include "rex_windows/engine/platform_creation_params.h"

// Include the auto tests defined by your project
#include "my_auto_tests/boot_test.h"

namespace regina
{
  rsl::array g_my_auto_tests =
  {
    // Declare auto tests here
    // The following is just an example
    rex::auto_test::AutoTest("Boot", my_auto_tests::boot_test_entry)
  };

} // namespace regina

namespace rex
{
  rex::ApplicationCreationParams app_entry(rex::PlatformCreationParams&& platformParams)
  {
    // Get the auto test cmd line argument. This holds the name of the auto test we want to run
    rsl::optional<rsl::string_view> cmdline = rex::cmdline::get_argument("AutoTest");

    // Make sure it's a known auto test
    REX_ASSERT_X(cmdline.has_value(), "Auto test fired but no auto test specified on the commandline. Commandline: {}", rex::cmdline::get());

    // Loop through our known auto tests and launch the one we want
    for (rex::auto_test::AutoTest& auto_test : regina::g_my_auto_tests)
    {
      if (auto_test.is_enabled(cmdline.value()))
      {
        return auto_test.launch(rsl::move(platformParams));
      }
    }

    // Assert if no valid auto test is found
    REX_ASSERT("No auto test found for {}", cmdline.value());

    // Return invalid app params if no valid auto test is found
    return rex::auto_test::invalid_app_params(rsl::move(platformParams));
  }
} // namespace rex
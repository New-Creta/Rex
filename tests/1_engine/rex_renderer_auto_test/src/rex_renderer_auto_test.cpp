#include "rex_engine/engine/entrypoint.h"
#include "rex_engine/engine/engine_params.h"

#include "rex_renderer_auto_test/rex_renderer_directx_initialize_test.h"
#include "rex_renderer_auto_test/rex_renderer_directx_build_clear_state_test.h"
#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_windows/platform_creation_params.h"

#include "rex_auto_test/auto_test.h"

namespace regina
{
  rsl::array g_auto_tests =  { rex::auto_test::AutoTest("DX12_Initialize", rex_renderer_directx_initialize_test::boot_test_entry),
                               rex::auto_test::AutoTest("DX12_ClearScreen", rex_renderer_directx_build_clear_state_test::boot_test_entry)};

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
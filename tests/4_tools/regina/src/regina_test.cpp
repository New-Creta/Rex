#include "rex_engine/entrypoint.h"
#include "rex_engine/engine_params.h"
#include "rex_windows/gui_application.h"

#include "regina_test/regina_boot_test.h"
#include "rex_engine/cmdline.h"
#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  rex::ApplicationCreationParams app_entry(rex::PlatformCreationParams&& platformParams)
  {
    rsl::optional<rsl::string_view> cmdline = cmdline::get_argument("AutoTest");

    REX_ASSERT_X(cmdline.has_value(), "Auto test fired but no auto test specified on the commandline");

    if (rsl::strincmp(cmdline.value().data(), "Boot", cmdline.value().length()) == 0)
    {
      return regina_test::boot_test_entry(rsl::move(platformParams));
    }

    return rex::ApplicationCreationParams(rsl::move(platformParams));
  }
} // namespace rex
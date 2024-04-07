#include "rex_engine/engine/entrypoint.h"
#include "rex_engine/engine/engine_params.h"

#include "regina_auto_test/regina_boot_test.h"
#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_windows/engine/platform_creation_params.h"

#include "rex_auto_test/auto_test.h"

namespace rex
{
  rsl::vector<auto_test::AutoTest> auto_test_entry()
  {
    return
    {
    rex::auto_test::AutoTest("Boot", regina_auto_test::boot_test_entry)
    };
  }
}
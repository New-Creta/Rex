#include "rex_engine/engine/entrypoint.h"
#include "rex_engine/engine/engine_params.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_auto_test/auto_test.h"

#include "rex_engine_auto_test/rex_engine_auto_test_vfs.h"

namespace rex
{
  namespace auto_test
  {
    rsl::vector<auto_test::AutoTest> auto_test_entry()
    {
      return
      {
        rex::auto_test::AutoTest("Vfs", auto_tests::vfs_test_entry)
      };
    }
  }
}
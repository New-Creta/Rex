#include "rex_engine/engine/entrypoint.h"
#include "rex_engine/engine/engine_params.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_auto_test/auto_test.h"

#include "rex_engine_auto_test/rex_engine_auto_test_console_app.h"
#include "rex_engine_auto_test/rex_engine_auto_test_gui_app.h"

namespace rex
{
  namespace auto_test
  {
    rsl::vector<auto_test::AutoTest> auto_test_entry()
    {
      return
      {
        rex::auto_test::AutoTest("ConsoleAppBootTest", console_app_boot_test::console_app_test_entry),
        rex::auto_test::AutoTest("GuiAppBootTest", gui_app_boot_test::gui_app_test_entry)
      };
    }
  }
}
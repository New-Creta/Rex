#include "rex_engine/engine/entrypoint.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/logging/logger_config.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/attributes.h"
#include "rex_std/internal/exception/exit.h"
#include "rex_std/thread.h"

#include "rex_engine/diagnostics/file_dialogs.h"

namespace rex
{
  namespace internal
  {
    void pre_app_entry(REX_MAYBE_UNUSED const tchar* cmdLine)
    {
      unsigned char lRgbColor[3];
      rex::dialog::tinyfd_colorChooser("choose a nice color", "#FF0077", lRgbColor, lRgbColor);
      rex::dialog::tinyfd_inputBox("Input Box", "This is an input box", rex::dialog::InputType::Default);
      rex::dialog::tinyfd_messageBox("Message box", "This is a msg box", rex::dialog::DialogType::Ok, rex::dialog::IconType::Info, rex::dialog::DefaultButton::Yes);
      rex::dialog::tinyfd_notifyPopup("Notify popup", "This is a notify popup", rex::dialog::IconType::Info);
      const char* filter[] = { "*.txt", "*.jpg" };
      rex::dialog::tinyfd_openFileDialog("Notify popup", "This is a notify popup", 1, filter, nullptr, true);
      rex::dialog::tinyfd_saveFileDialog("Notify popup", "This is a notify popup", 1, filter, nullptr);
      rex::dialog::tinyfd_selectFolderDialog("Notify popup", "D:\\");


      cmdline::init(rsl::wstring_view(cmdLine));

      // if a user wants to know the arguments for the executable, we want to perform as minimal setup as possible.
      // we just initialize the commandline, print what's possible and exit the program
      if(cmdline::get_argument(L"help"))
      {
        cmdline::print_args();
        rsl::exit(0);
      }

      // if we want to debug executables without a debugger, we need to break early
      // so we can attach a debugger and continue from then on
      // we'll have a timer in place to break for 2 minutes, if no debugger is attached
      // we close down the program
      if(cmdline::get_argument(L"BreakOnBoot"))
      {
        using namespace rsl::chrono_literals; // NOLINT(google-build-using-namespace)
        auto i = 1s;
        while(i < 10min)
        {
          rsl::this_thread::sleep_for(1s);
          ++i;
        }

        // when the debugger is attached, skip this line
        rsl::exit(0);
      }

      diagnostics::init_log_levels();
      vfs::init();
    }

    void post_app_shutdown()
    {
      vfs::shutdown();

      cmdline::shutdown();
    }
  } // namespace internal
} // namespace rex
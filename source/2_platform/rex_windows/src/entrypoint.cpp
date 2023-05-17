#include "rex_engine/entrypoint.h"

#include "rex_engine/cmd_line_args.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/types.h"
#include "rex_std/bonus/utility.h"
#include "rex_windows/gui_application.h"
#include "rex_windows/log.h"
#include "rex_windows/platform_creation_params.h"

#define NOMINMAX
#include <Windows.h>
#include <libloaderapi.h>
#include <processenv.h>
#include <winuser.h>

//-------------------------------------------------------------------------
INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
  rex::CommandLineArguments cmd_args(GetCommandLineA());
  rex::PlatformCreationParams creation_params {};
  creation_params.instance      = hInstance;
  creation_params.prev_instance = hPrevInstance;
  creation_params.cmd_line      = lpCmdLine;
  creation_params.show_cmd      = nShowCmd;

  rex::ApplicationCreationParams app_params = rex::app_entry(rsl::move(creation_params), rsl::move(cmd_args));

  // this doesn't initialize anything but simply prepares the application for initialization
  rex::win32::GuiApplication application(rsl::move(app_params));

  // this initializes, runs the loop and performs the shutdown
  s32 result = application.run();

  // by this point the application has finished and shutdown
  REX_LOG(LogWindows, "Application completed with result: {0}", result);

  return result;
}

int main()
{
  STARTUPINFOW si;
  GetStartupInfoW(&si);

  s32 show_window = si.wShowWindow;

  if(!rsl::has_flag(si.dwFlags, STARTF_USESHOWWINDOW)) // this happens when running from the debugger
  {
    show_window = SW_SHOWNORMAL;
  }

  return WinMain(GetModuleHandle(nullptr), nullptr, GetCommandLine(), show_window);
}

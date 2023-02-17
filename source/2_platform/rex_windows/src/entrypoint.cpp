#include "rex_engine/entrypoint.h"

#include "rex_engine/diagnostics/logging.h"
#include "rex_std/bonus/utility/has_flag.h"
#include "rex_std/string_view.h"
#include "rex_std/vector.h"
#include "rex_windows/gui_application.h"
#include "rex_windows/platform_creation_params.h"
#include "rex_windows/win_types.h"

#define NOMINMAX
#include <Windows.h>
#include <cstdlib>
#include <iostream>
#include <shellapi.h>

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

  rex::win32::GuiApplication application(app_params);
  s32 result = application.run();

  REX_INFO("Application completed with result: {0}", result);

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

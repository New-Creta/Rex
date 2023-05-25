#include "rex_engine/entrypoint.h"

#include "rex_engine/cmd_line_args.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/types.h"
#include "rex_std/bonus/utility.h"
#include "rex_windows/console_application.h"
#include "rex_windows/gui_application.h"
#include "rex_windows/log.h"
#include "rex_windows/platform_creation_params.h"

#define NOMINMAX
#include <Windows.h>
#include <cstdlib>
#include <iostream>
#include <shellapi.h>

//-------------------------------------------------------------------------
INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR /*lpCmdLine*/, int nShowCmd)
{
  rex::PlatformCreationParams creation_params {};
  creation_params.instance      = hInstance;
  creation_params.prev_instance = hPrevInstance;
  creation_params.show_cmd      = nShowCmd;

  rex::ApplicationCreationParams app_params = rex::app_entry(rsl::move(creation_params));

  s32 result = 0;
  if(app_params.create_window)
  {
    // this doesn't initialize anything but simply prepares the application for initialization
    rex::win32::GuiApplication application(rsl::move(app_params));

    // this initializes, runs the loop and performs the shutdown
    result = application.run();
  }
  else
  {
    // this doesn't initialize anything but simply prepares the application for initialization
    rex::win32::ConsoleApplication application(rsl::move(app_params));

    // this initializes, runs the loop and performs the shutdown
    result = application.run();
  }

  // by this point the application has finished and shutdown
  REX_LOG(LogWindows, "Application completed with result: {0}", result);

  return result;
}

// main is always the entry point.
// on a graphical application however, we call into WinMain
// as this is supposed to be the entry point for a graphical application
// This is also the entry point that will be used without a console.
int main()
{
  rex::internal::pre_app_entry(GetCommandLine());

  STARTUPINFOW si;
  GetStartupInfoW(&si);

  s32 show_window = si.wShowWindow;

  if(!rsl::has_flag(si.dwFlags, STARTF_USESHOWWINDOW)) // this happens when running from the debugger
  {
    show_window = SW_SHOWNORMAL;
  }

  const int result = WinMain(GetModuleHandle(nullptr), nullptr, GetCommandLine(), show_window);

  rex::internal::post_app_shutdown();

  return result;
}

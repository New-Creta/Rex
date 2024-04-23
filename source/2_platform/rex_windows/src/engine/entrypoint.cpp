#include "rex_engine/engine/entrypoint.h"

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/platform/win/crash_reporter/win_crash_handler.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/utility.h"
#include "rex_std/iostream.h"
#include "rex_windows/app/console_application.h"
#include "rex_windows/app/gui_application.h"
#include "rex_windows/diagnostics/log.h"
#include "rex_windows/engine/platform_creation_params.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <processenv.h>

//-------------------------------------------------------------------------
int rex_entry(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPCSTR lpCmdLine, int nShowCmd)
{
  rex::internal::pre_app_entry(lpCmdLine);

  rex::PlatformCreationParams creation_params {};
  creation_params.instance      = hInstance;
  creation_params.prev_instance = hPrevInstance;
  creation_params.show_cmd      = nShowCmd;

  s32 result = 0;

  // We're wrapping executing in __try - __except block so that we can capture crashes
  // This allows us to perform particular logic when we encounter a crash.
  // Especially for when running tests, this can be very useful
  // See: https://learn.microsoft.com/en-us/cpp/cpp/try-except-statement?view=msvc-170
  __try
  {
    rex::ApplicationCreationParams app_params = rex::app_entry(rsl::move(creation_params));

    if(app_params.create_window)
    {
      // this doesn't initialize anything but simply prepares the application for initialization
      rex::win::GuiApplication application(rsl::move(app_params));

      // this initializes, runs the loop and performs the shutdown
      result = application.run();
    }
    else
    {
      // this doesn't initialize anything but simply prepares the application for initialization
      rex::win::ConsoleApplication application(rsl::move(app_params));

      // this initializes, runs the loop and performs the shutdown
      result = application.run();
    }
  }
  __except(rex::win::report_crash_from_main_thread(GetExceptionInformation()), EXCEPTION_CONTINUE_SEARCH)
  {
    // Do nothing here as the code here doesn't get executed due to EXCEPTION_CONTINUE_SEARCH
    // handle crashing in the report_crash() function, then exit
    (void)0;
  }

  // by this point the application has finished and shutdown
  REX_LOG(LogWindows, "Application completed with result: {0}", result);

  rex::internal::post_app_shutdown();

  return result;
}

//-------------------------------------------------------------------------
INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  return rex_entry(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}

// main is always the entry point.
// on a graphical application however, we call into WinMain
// as this is supposed to be the entry point for a graphical application
// This is also the entry point that will be used without a console.
int main()
{
  STARTUPINFOW si;
  GetStartupInfoW(&si);

  s32 show_window = si.wShowWindow;

  if(!rsl::has_flag(si.dwFlags, STARTF_USESHOWWINDOW)) // this happens when running from the debugger
  {
    show_window = SW_SHOWNORMAL;
  }

  const int result = rex_entry(GetModuleHandle(nullptr), nullptr, GetCommandLineA(), show_window);

  return result;
}

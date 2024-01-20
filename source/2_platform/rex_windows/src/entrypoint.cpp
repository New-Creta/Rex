#include "rex_engine/engine/entrypoint.h"

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/utility.h"
#include "rex_std/iostream.h"
#include "rex_windows/console_application.h"
#include "rex_windows/gui_application.h"
#include "rex_windows/log.h"
#include "rex_windows/platform_creation_params.h"

#define NOMINMAX
#include <Windows.h>
#include <processenv.h>

//-------------------------------------------------------------------------
s32 report_crash(LPEXCEPTION_POINTERS ExceptionInfo)
{
  rsl::cout << "We crashed!\n";
  (void)ExceptionInfo;
  return EXCEPTION_EXECUTE_HANDLER;
}

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
  __try
  {
    rex::ApplicationCreationParams app_params = rex::app_entry(rsl::move(creation_params));

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
  }
  __except(report_crash(GetExceptionInformation()), EXCEPTION_CONTINUE_SEARCH)
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

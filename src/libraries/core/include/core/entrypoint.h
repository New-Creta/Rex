#pragma once

#include "core/application.h"

#include "rex_debug.h"

#include <memory>

#ifdef _WINDOWS

#include <Windows.h>

//--------------------------------------------------------------------------------------------
extern rex::CoreApplication* rex::createApplication();

//--------------------------------------------------------------------------------------------
// Windows entry point
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR pCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(pCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    // Before we start the application we initialize the logger
    RX_INITIALIZE_LOGGER();

    std::unique_ptr<rex::CoreApplication> application(rex::createApplication());

    int result = application->run();

    // Before we close down the application we shutdown the logger.
    RX_SHUTDOWN_LOGGER();

    return result;
}

//--------------------------------------------------------------------------------------------
// Universal entry point
int main(int argc, char** argv)
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    HINSTANCE   h_instance = GetModuleHandle(NULL);  // The operating system uses this value to identify the executable (EXE/DLL) when it is loaded in memory.
    HINSTANCE   h_prev_instance = NULL;              // Has no meaning, it was used in 16-bit Windows, but is now always zero.
    LPTSTR      p_cmd_line = NULL;                   // Contains the command-line arguments as a Unicode string.
    int         n_cmd_show = 1;                      // A flag that says whether the main application window will be minimized, maximized, or shown normally.

    return WinMain(h_instance, h_prev_instance, p_cmd_line, n_cmd_show);
}

#endif
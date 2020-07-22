#pragma once

// rex - core
#include "core/application.h"

// rex - diagnostics
#include "logger.h"

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

    std::unique_ptr<rex::CoreApplication> application(rex::createApplication());

    return application->run();
}

//--------------------------------------------------------------------------------------------
// Universal entry point
int main(int argc, char** argv)
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    rex::Logger::initialize(rex::LogLevel::INFO);

    rex::Logger::getLogger(rex::Logger::ENGINE_LOGGER_NAME).log(spdlog::level::level_enum::trace,   "Trace Test");
    rex::Logger::getLogger(rex::Logger::ENGINE_LOGGER_NAME).log(spdlog::level::level_enum::info,    "Info Test");
    rex::Logger::getLogger(rex::Logger::ENGINE_LOGGER_NAME).log(spdlog::level::level_enum::warn,    "Warning Test");
    rex::Logger::getLogger(rex::Logger::ENGINE_LOGGER_NAME).log(spdlog::level::level_enum::err,     "Error Test");

    HINSTANCE   h_instance = GetModuleHandle(NULL);  // The operating system uses this value to identify the executable (EXE/DLL) when it is loaded in memory.
    HINSTANCE   h_prev_instance = NULL;              // Has no meaning, it was used in 16-bit Windows, but is now always zero.
    LPTSTR      p_cmd_line = NULL;                   // Contains the command-line arguments as a Unicode string.
    int         n_cmd_show = 1;                      // A flag that says whether the main application window will be minimized, maximized, or shown normally.

    return WinMain(h_instance, h_prev_instance, p_cmd_line, n_cmd_show);
}

#endif
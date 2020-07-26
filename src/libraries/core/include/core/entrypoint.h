#pragma once

#include "rex_core_pch.h"

#include "core/coreapplication.h"

#ifdef _WINDOWS

//--------------------------------------------------------------------------------------------
extern rex::CoreApplication* rex::createApplication();

//--------------------------------------------------------------------------------------------
// Universal entry point
int main(int argc, char** argv)
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    // Before we start the application we initialize the logger
    RX_INITIALIZE_LOGGER();

    std::unique_ptr<rex::CoreApplication> application(rex::createApplication());

    int result = application->run();

    // Before we close down the application we shutdown the logger.
    RX_SHUTDOWN_LOGGER();

    return result;
}

#endif
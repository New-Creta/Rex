#pragma once

#include "core_application.h"

//--------------------------------------------------------------------------------------------
extern rex::CoreApplication* rex::create_application();

//--------------------------------------------------------------------------------------------
int run_program(int /*argc*/, char** /*argv*/)
{
    rex::CoreApplication* application = rex::create_application();

    int result = application->run();

    delete application;

    return result;
}

#if defined REX_PLATFORM_WINDOWS

//--------------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    int result = run_program(argc, argv);

    return result;
}

#else

    #error Unsupported platform

#endif
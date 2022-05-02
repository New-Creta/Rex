#pragma once

#include "core_application.h"
#include "application_arguments.h"

//--------------------------------------------------------------------------------------------
extern rex::CoreApplication* rex::create_application(const ApplicationArguments& arguments);

//--------------------------------------------------------------------------------------------
int run_program(int argc, char** argv)
{
    rex::ApplicationArguments application_arguments(argc, argv);

    rex::CoreApplication* application = rex::create_application(application_arguments);

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
#include "core/entrypoint.h"
#include "core/application.h"

//-------------------------------------------------------------------------
sandbox::Application::Application() = default;
//-------------------------------------------------------------------------
sandbox::Application::~Application() = default;

//-------------------------------------------------------------------------
rex::CoreApplication* rex::createApplication()
{
    return new sandbox::Application();
}
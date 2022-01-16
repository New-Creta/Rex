#include "regina_pch.h"
#include "entrypoint.h"
#include "regina.h"

//-----------------------------------------------------------------------
rex::CoreApplication* rex::createApplication()
{
    rex::ApplicationDescription description;

    description.max_fps = 60u;

    return new regina::Regina(description);
}

namespace regina
{
    //-------------------------------------------------------------------------
    Application::Application(const rex::ApplicationDescription& description)
        : rex::GraphicalApplication(description)
    {
    }
    //-------------------------------------------------------------------------
    Application::~Application() = default;
}
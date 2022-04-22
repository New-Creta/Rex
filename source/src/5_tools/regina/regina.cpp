#include "regina_pch.h"
#include "entrypoint.h"
#include "window.h"
#include "regina.h"

//-----------------------------------------------------------------------
rex::CoreApplication* rex::create_application()
{
    rex::ApplicationDescription description;

    description.max_fps = 60u;

    return new regina::Regina(description);
}

//-----------------------------------------------------------------------
rex::CoreWindow* rex::create_window()
{
    rex::WindowDescription description;

    description.title = "Regina";
    description.fullscreen = rex::FullScreen::No;
    description.width = 1280;
    description.height = 720;

    return new rex::Window(description);
}

namespace regina
{
    //-------------------------------------------------------------------------
    Regina::Regina(const rex::ApplicationDescription& description)
        : rex::GraphicalApplication(description)
    {
    }
    //-------------------------------------------------------------------------
    Regina::~Regina() = default;
}
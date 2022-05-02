#include "regina_pch.h"
#include "entrypoint.h"
#include "win32_window.h"
#include "regina.h"

//-----------------------------------------------------------------------
rex::CoreApplication* rex::create_application()
{
    rex::ApplicationDescription description;

    description.max_fps = 60u;

    return new regina::Regina(description);
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
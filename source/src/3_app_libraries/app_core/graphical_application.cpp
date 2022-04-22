#include "app_core_pch.h"

#include "graphical_application.h"

namespace rex
{
    //-------------------------------------------------------------------------
    GraphicalApplication::GraphicalApplication(const ApplicationDescription& description)
        :GuiApplication(description)
    {}
    //-------------------------------------------------------------------------
    GraphicalApplication::~GraphicalApplication() = default;

    //-------------------------------------------------------------------------
    void GraphicalApplication::app_initialize()
    {
        initialize();
    }
    //-------------------------------------------------------------------------
    void GraphicalApplication::app_update(const FrameInfo& info)
    {
        update(info);
    }
    //-------------------------------------------------------------------------
    void GraphicalApplication::app_shutdown()
    {
        shutdown();
    }
}
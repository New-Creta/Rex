#include "rex_application_core_pch.h"

#include "graphical_application.h"

namespace rex
{
    //-------------------------------------------------------------------------
    GraphicalApplication::GraphicalApplication(const ApplicationDescription& description)
        :win32::Application(description)
    {}
    //-------------------------------------------------------------------------
    GraphicalApplication::~GraphicalApplication() = default;

    //-------------------------------------------------------------------------
    void GraphicalApplication::on_app_initialize()
    {
        initialize();
    }
    //-------------------------------------------------------------------------
    void GraphicalApplication::on_app_update(const FrameInfo& info)
    {
        update(info);
    }
    //-------------------------------------------------------------------------
    void GraphicalApplication::on_app_shutdown()
    {
        shutdown();
    }
}
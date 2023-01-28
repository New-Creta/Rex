#include "rex_application_core/graphical_application.h"
#include "rex_engine/diagnostics/logging.h"
#include "rex_renderer/renderer.h"

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
        if (!renderer::initialize())
        {
            REX_ERROR("The renderer did not initialize correctly.");
            return;
        }

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
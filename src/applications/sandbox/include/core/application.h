#pragma once

#include "core/graphicalapplication.h"

namespace rex
{
    class Window;
}

namespace sandbox
{
    class Application : public rex::engine::GraphicalApplication
    {
    public:
        Application();
        ~Application() override;

    protected:
        void appInitialize() override;
    };
}
#pragma once

#include "core/coreapplication.h"

namespace rex
{
    class Window;
}

namespace sandbox
{
    class Application : public rex::CoreApplication
    {
    public:
        Application();
        ~Application() override;

    protected:
        std::unique_ptr<rex::Window> createWindow() override;
    };
}
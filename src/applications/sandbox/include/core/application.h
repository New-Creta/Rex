#pragma once

#include "core/coreapplication.h"

namespace sandbox
{
    class Application : public rex::CoreApplication
    {
    public:
        Application();
        ~Application() override;
    };
}
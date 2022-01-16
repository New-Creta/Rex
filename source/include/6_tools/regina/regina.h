#pragma once

#include "graphical_application.h"

namespace regina
{
    class Regina : public rex::GraphicalApplication
    {
    public:
        Regina(const engine::ApplicationDescription& description);
        ~Regina() override;
    }
}
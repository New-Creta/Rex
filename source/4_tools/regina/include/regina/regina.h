#pragma once

#include "rex_application_core/graphical_application.h"

namespace regina
{
    class Regina : public rex::GraphicalApplication
    {
    public:
        Regina(const rex::ApplicationDescription& description);
        ~Regina() override;
    };
}
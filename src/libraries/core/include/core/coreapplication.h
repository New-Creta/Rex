#pragma once

#include "rex_core_global.h"

namespace rex
{
    class CoreApplication
    {
    public:
        REX_CORE_EXPORT CoreApplication();
        REX_CORE_EXPORT virtual ~CoreApplication();

        REX_CORE_EXPORT int run();
    };

    // 
    // This will be implemented by the CLIENT
    //
    CoreApplication* createApplication();
}
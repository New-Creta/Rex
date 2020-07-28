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
        REX_CORE_EXPORT void quit();

    protected:
        REX_CORE_EXPORT void markForDestroy();

        //
        // Application runtime
        //
        REX_CORE_EXPORT virtual void platformInitialize() = 0;
        REX_CORE_EXPORT virtual void platformUpdate(float dTime) = 0;
        REX_CORE_EXPORT virtual void platformQuit() = 0;

    private:
        bool m_running;

        // There will only be 1 application running at the time.
        static CoreApplication* s_application_instance;
    };

    // 
    // This will be implemented by the CLIENT
    //
    CoreApplication* createApplication();
}
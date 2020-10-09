#pragma once

#include "rex_core_global.h"

#include "core/deltatime.h"

namespace rex
{
    class CoreApplication
    {
    public:
        REX_CORE_EXPORT CoreApplication();
        REX_CORE_EXPORT virtual ~CoreApplication();

        REX_CORE_EXPORT int run();
        REX_CORE_EXPORT void quit();

        static CoreApplication* getInstance() { return s_application_instance; }

    protected:
        REX_CORE_EXPORT void markForDestroy();

        //
        // Application runtime
        //
        REX_CORE_EXPORT virtual void platformInitialize() = 0;
        REX_CORE_EXPORT virtual void platformUpdate(const DeltaTime& dTime) = 0;
        REX_CORE_EXPORT virtual void platformQuit() = 0;

    private:
        bool m_running;

        std::chrono::steady_clock::time_point m_last_time_point;

        // There will only be 1 application running at the time.
        REX_CORE_EXPORT static CoreApplication* s_application_instance;
    };

    // 
    // This will be implemented by the CLIENT
    //
    CoreApplication* createApplication();
}
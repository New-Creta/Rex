#pragma once

#include "rex_core_global.h"

namespace rex
{
    namespace events
    {
        class Event;
    }

    class Window;

    class CoreApplication
    {
    public:
        REX_CORE_EXPORT CoreApplication();
        REX_CORE_EXPORT virtual ~CoreApplication();

        REX_CORE_EXPORT int run();

    protected:
        REX_CORE_EXPORT void onEvent(const events::Event& event);

        virtual std::unique_ptr<rex::Window> createWindow() = 0;

    private:
        std::unique_ptr<Window> m_window;
    };

    // 
    // This will be implemented by the CLIENT
    //
    CoreApplication* createApplication();
}
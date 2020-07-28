#pragma once

#include "rex_engine_global.h"

#include "core/applicationconfig.h"
#include "core/windowconfig.h"

namespace rex
{
    namespace events
    {
        class Event;
    }

    namespace engine
    {
        class GraphicalApplication : public BaseApplication
        {
        public:
            REX_ENGINE_EXPORT GraphicalApplication();
            REX_ENGINE_EXPORT ~GraphicalApplication() override;

            REX_ENGINE_EXPORT void onEvent(const events::Event& event);

        protected:
            REX_ENGINE_EXPORT void appInitialize() override;
            REX_ENGINE_EXPORT void appUpdate(float dTime) override;
            REX_ENGINE_EXPORT void appQuit() override;

            REX_ENGINE_EXPORT ApplicationWindow* getWindow();
            REX_ENGINE_EXPORT const ApplicationWindow* getWindow() const;

        private:
            std::unique_ptr<ApplicationWindow> createWindow();

            std::unique_ptr<ApplicationWindow> m_window;
        };
    }
}
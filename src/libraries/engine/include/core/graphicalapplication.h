#pragma once

#include "rex_engine_global.h"

#include "core/layerstack.h"

#include "config/config.h"

namespace rex
{
    namespace events
    {
        class Event;
        class KeyDown;
    }

    namespace engine
    {
        class GraphicalApplication : public BaseApplication
        {
        public:
            REX_ENGINE_EXPORT GraphicalApplication();
            REX_ENGINE_EXPORT ~GraphicalApplication() override;

            REX_ENGINE_EXPORT Window* getWindow() override;
            REX_ENGINE_EXPORT const Window* getWindow() const override;

            REX_ENGINE_EXPORT void pushBackLayer(std::unique_ptr<Layer> layer);
            REX_ENGINE_EXPORT void pushFrontLayer(std::unique_ptr<Layer> layer);

            REX_ENGINE_EXPORT void onEvent(events::Event& event);

        protected:
            REX_ENGINE_EXPORT void appInitialize() override;
            REX_ENGINE_EXPORT void appUpdate(float dTime) override;
            REX_ENGINE_EXPORT void appQuit() override;

        private:
            std::unique_ptr<ApplicationWindow> createWindow();
            std::unique_ptr<ApplicationContext> createContext();

            std::unique_ptr<ApplicationWindow> m_window;
            std::unique_ptr<ApplicationContext> m_context;

            LayerStack m_layer_stack;
        };
    }
}
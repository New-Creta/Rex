#pragma once

#include "rex_engine_global.h"

#include "core/layerstack.h"
#include "core/orthographiccamera.h"

#include "config/config.h"

namespace rex
{
    namespace events
    {
        class Event;
        class WindowClose;
    }

    namespace graphics
    {
        class Graphics;

        class Shader;
        class Mesh;

        class VertexArray;
        class VertexBuffer;
        class IndexBuffer;
    }

    namespace engine
    {
        class ImGUILayer;

        class GraphicalApplication : public BaseApplication
        {
        public:
            REX_ENGINE_EXPORT GraphicalApplication();
            REX_ENGINE_EXPORT ~GraphicalApplication() override;

            REX_ENGINE_EXPORT Window* getWindow() override;
            REX_ENGINE_EXPORT const Window* getWindow() const override;
            REX_ENGINE_EXPORT rex::graphics::Context* getContext() override;
            REX_ENGINE_EXPORT const rex::graphics::Context* getContext() const override;

            REX_ENGINE_EXPORT void pushBackLayer(std::unique_ptr<Layer> layer);
            REX_ENGINE_EXPORT void pushFrontLayer(std::unique_ptr<Layer> layer);

            REX_ENGINE_EXPORT void onEvent(events::Event& event);

        protected:
            REX_ENGINE_EXPORT void appInitialize() final;
            REX_ENGINE_EXPORT void appUpdate(float dTime) final;
            REX_ENGINE_EXPORT void appQuit() final;

            virtual void onInitialize() {}
            virtual void onUpdate(float dTime) { UNUSED_PARAM(dTime); }
            virtual void onQuit() {}

        private:
            bool onWindowClose(events::WindowClose& e);

            std::unique_ptr<ApplicationWindow> createWindow();
            std::unique_ptr<ApplicationContext> createContext();
            std::unique_ptr<ImGUILayer> createImGUILayer();

            std::unique_ptr<ApplicationWindow> m_window;
            std::unique_ptr<ApplicationContext> m_context;

            LayerStack m_layer_stack;
        };
    }
}
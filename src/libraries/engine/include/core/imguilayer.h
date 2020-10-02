#pragma once

#include "rex_engine_global.h"

#include "core/layer.h"

namespace rex
{
    namespace events
    {
        class MouseDown;
        class MouseUp;
        class MouseScroll;
        class MouseMove;

        class KeyDown;
        class KeyUp;
        class KeyTyped;

        class WindowResize;
    }

    namespace engine
    {
        class ImGUILayer : public Layer
        {
        public:
            REX_ENGINE_EXPORT ImGUILayer();
            REX_ENGINE_EXPORT ~ImGUILayer() override;

            REX_ENGINE_EXPORT void onAttach() override;
            REX_ENGINE_EXPORT void onDetach() override;

            REX_ENGINE_EXPORT void onEvent(events::Event& event) override;
            
            REX_ENGINE_EXPORT virtual void onBeginRender();
            REX_ENGINE_EXPORT virtual void onRender();
            REX_ENGINE_EXPORT virtual void onEndRender();

        private:
            bool onProcessImGuiEvent(events::Event& event);

            ApplicationWindow* m_window;
            ApplicationContext* m_context;

            bool m_imgui_context_created;
        };
    }
}
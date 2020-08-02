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

            REX_ENGINE_EXPORT void onUpdate() override;
            REX_ENGINE_EXPORT void onEvent(events::Event& event) override;

        private:
            bool onMouseButtonPressedEvent(events::MouseDown& e);
            bool onMouseButtonReleasedEvent(events::MouseUp& e);
            bool onMouseMovedEvent(events::MouseMove& e);
            bool onMouseScrolledEvent(events::MouseScroll& e);
            bool onKeyPressedEvent(events::KeyDown& e);
            bool onKeyReleasedEvent(events::KeyUp& e);
            bool onKeyTypedEvent(events::KeyTyped& e);
            bool onWindowResizedEvent(events::WindowResize& e);

            ApplicationWindow* m_window;

            bool m_imgui_context_created;
        };
    }
}
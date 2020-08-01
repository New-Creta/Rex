#pragma once

#include "rex_engine_global.h"

#include "core/layer.h"

namespace rex
{
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
            ApplicationWindow* m_window;

            bool m_imgui_context_created;
        };
    }
}
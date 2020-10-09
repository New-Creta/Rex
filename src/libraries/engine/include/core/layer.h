#pragma once

#include "rex_engine_global.h"

namespace rex
{
    namespace events
    {
        class Event;
    }

    namespace engine
    {
        class Layer
        {
        public:
            REX_ENGINE_EXPORT Layer(const std::string& name = "Layer");
            REX_ENGINE_EXPORT virtual ~Layer();

            REX_ENGINE_EXPORT void enable();
            REX_ENGINE_EXPORT void disable();

            virtual void onAttach() {};
            virtual void onDetach() {};

            REX_ENGINE_EXPORT virtual void onUpdate(const DeltaTime& dTime);
            REX_ENGINE_EXPORT virtual void onEvent(events::Event& event);

            inline const std::string& getName() const { return m_name; }

        private:
            std::string m_name;
            bool m_enabled;
        };
    }
}
#pragma once

#include "event.h"

#include "core/keycodes.h"

namespace rex
{
    namespace events
    {
        class KeyEvent : public Event
        {
        public:
            EVENT_CLASS_CATEGORY(EventCategory::INPUT | EventCategory::KEYBOARD);
            
            ~KeyEvent() override;  

            KeyCode getKeyCode() const;

        protected:
            KeyEvent(KeyCode code);

        private:
            KeyCode m_code;
        };
    }
}
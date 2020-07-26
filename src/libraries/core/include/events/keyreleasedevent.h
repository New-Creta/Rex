#pragma once

#include "keyevent.h"

namespace rex
{
    namespace events
    {
        class KeyReleasedEvent : public KeyEvent
        {
        public:
            EVENT_CLASS_TYPE(EventType::KEY_RELEASED);

            KeyReleasedEvent(KeyCode code);
            ~KeyReleasedEvent() override;

            std::string toString() const override;
        };
    }
}
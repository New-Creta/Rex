#pragma once

#include "mousebuttonevent.h"

namespace rex
{
    namespace events
    {
        class MouseButtonPressedEvent : public MouseButtonEvent
        {
        public:
            EVENT_CLASS_TYPE(EventType::MOUSE_BUTTON_PRESSED);

            MouseButtonPressedEvent(MouseCode code, int x, int y);
            ~MouseButtonPressedEvent() override;

            std::string toString() const override;
        };
    }
}
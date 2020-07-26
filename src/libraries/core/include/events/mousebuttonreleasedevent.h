#pragma once

#include "mousebuttonevent.h"

namespace rex
{
    namespace events
    {
        class MouseButtonReleasedEvent : public MouseButtonEvent
        {
        public:
            EVENT_CLASS_TYPE(EventType::MOUSE_BUTTON_RELEASED);

            MouseButtonReleasedEvent(MouseCode code, int x, int y);
            ~MouseButtonReleasedEvent() override;

            std::string toString() const override;
        };
    }
}
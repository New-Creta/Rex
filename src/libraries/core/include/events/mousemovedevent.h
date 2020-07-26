#pragma once

#include "mouseevent.h"

namespace rex
{
    namespace events
    {
        class MouseMovedEvent : public MouseEvent
        {
        public:
            EVENT_CLASS_TYPE(EventType::MOUSE_MOVED);

            MouseMovedEvent(int x, int y);
            ~MouseMovedEvent() override;

            std::string toString() const override;
        };
    }
}
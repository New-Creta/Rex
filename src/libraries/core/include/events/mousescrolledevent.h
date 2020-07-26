#pragma once

#include "mouseevent.h"

namespace rex
{
    namespace events
    {
        class MouseScrolledEvent : public MouseEvent
        {
        public:
            EVENT_CLASS_TYPE(EventType::MOUSE_SCROLLED);

            MouseScrolledEvent(int deltaX, int deltaY, int x, int y);
            ~MouseScrolledEvent() override;

            int getScollX() const;
            int getScollY() const;

            std::string toString() const override;

        private:
            int m_delta_x;
            int m_delta_y;
        };
    }
}
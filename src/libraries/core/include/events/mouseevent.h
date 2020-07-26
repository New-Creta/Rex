#pragma once

#include "event.h"

namespace rex
{
    namespace events
    {
        class MouseEvent : public Event
        {
        public:
            EVENT_CLASS_CATEGORY(EventCategory::INPUT | EventCategory::MOUSE);
            
            ~MouseEvent() override;

            int getX() const;
            int getY() const;

        protected:
            MouseEvent(int x, int y);

        private:
            int m_x;
            int m_y;
        };
    }
}
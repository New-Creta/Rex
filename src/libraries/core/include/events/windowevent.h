#pragma once

#include "event.h"

namespace rex
{
    namespace events
    {
        class WindowEvent : public Event
        {
        public:
            EVENT_CLASS_CATEGORY(EventCategory::WINDOW);

            ~WindowEvent() override;

        protected:
            WindowEvent();
        };
    }
}
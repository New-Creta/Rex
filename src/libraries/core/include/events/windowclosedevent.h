#pragma once

#include "windowevent.h"

namespace rex
{
    namespace events
    {
        class WindowClosedEvent : public WindowEvent
        {
        public:
            EVENT_CLASS_TYPE(EventType::WINDOW_CLOSED);

            WindowClosedEvent();
            ~WindowClosedEvent() override;

            std::string toString() const override;
        };
    }
}
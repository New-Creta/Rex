#pragma once

#include "event.h"

namespace rex
{
    namespace events
    {
        class WindowEvent : public BaseEvent
        {
        public:
            EVENT_CLASS_CATEGORY(EventCategory::WINDOW);

            WindowEvent(uint32 windowID/* , const memory::Blob& nativeEvent */);
            ~WindowEvent() override;

            uint32 get_window_id() const;

        private:
            uint32 m_window_id;
        };
    }
}
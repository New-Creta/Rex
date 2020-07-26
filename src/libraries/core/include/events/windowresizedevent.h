#pragma once

#include "windowevent.h"

namespace rex
{
    namespace events
    {
        class WindowResizedEvent : public WindowEvent
        {
        public:
            EVENT_CLASS_TYPE(EventType::WINDOW_RESIZE);

            WindowResizedEvent(int width, int height);
            ~WindowResizedEvent() override;

            int getWidth() const;
            int getHeight() const;

            std::string toString() const override;

        private:
            int m_width;
            int m_height;
        };
    }
}
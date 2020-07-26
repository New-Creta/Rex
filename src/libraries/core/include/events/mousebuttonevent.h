#pragma once

#include "mouseevent.h"

#include "core/mousecodes.h"

namespace rex
{
    namespace events
    {
        class MouseButtonEvent : public MouseEvent
        {
        public:
            MouseCode getMouseCode() const;

        protected:
            MouseButtonEvent(MouseCode code, int x, int y);
            ~MouseButtonEvent() override;

        private:
            MouseCode m_code;
        };
    }
}
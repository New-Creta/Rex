#pragma once

#include "keyevent.h"

namespace rex
{
    namespace events
    {
        class KeyPressedEvent : public KeyEvent
        {
        public:
            EVENT_CLASS_TYPE(EventType::KEY_PRESSED);

            KeyPressedEvent(KeyCode code, int repeat);
            ~KeyPressedEvent() override;

            int getRepeatCount() const;

            std::string toString() const override;

        private:
            int m_repeat_count;
        };
    }
}
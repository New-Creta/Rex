#pragma once

#include "events/input/inputevent.h"

namespace rex
{
    namespace events
    {
        class KeyEvent : public InputEvent
        {
        public:
            EVENT_CLASS_CATEGORY(EventCategory::INPUT | EventCategory::KEYBOARD);

            ~KeyEvent() override;

            REX_CORE_EXPORT const int8* getKey() const;

        protected:
            KeyEvent(rex::Window* window, const int8* key);

        private:
            const int8* m_key;
        };
    }
}
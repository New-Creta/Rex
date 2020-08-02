#pragma once

#include "rex_core_global.h"

#include "keyevent.h"

namespace rex
{
    namespace events
    {
        class KeyTyped : public KeyEvent
        {
        public:
            EVENT_CLASS_TYPE(EventType::KEY_TYPED);

            REX_CORE_EXPORT KeyTyped(rex::Window* window, const int8* key);
            REX_CORE_EXPORT ~KeyTyped() override;

            REX_CORE_EXPORT std::string toString() const override;
        };
    }
}
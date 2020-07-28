#pragma once

#include "rex_core_global.h"

#ifdef _WINDOWS
    #undef MOUSE_MOVED
#endif

namespace rex
{
    namespace events
    {
        class EventType
        {
        public:
            enum Type
            {
                NONE = 0,
                WINDOW_CLOSED, WINDOW_RESIZE, WINDOW_ACTIVATED, WINDOW_DEACTIVATED, WINDOW_OPEN,
                KEY_PRESSED, KEY_RELEASED,
                MOUSE_BUTTON_PRESSED, MOUSE_BUTTON_RELEASED, MOUSE_MOVED, MOUSE_SCROLLED
            };

            REX_CORE_EXPORT EventType(Type value);

            REX_CORE_EXPORT operator Type() const;

            REX_CORE_EXPORT uint32 toUint() const;
            REX_CORE_EXPORT std::string toString() const;

        private:
            Type m_value;
        };
    }
}
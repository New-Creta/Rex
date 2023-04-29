#pragma once

#include "rex_engine/types.h"
#include "rex_engine/event_type.h"

namespace rex
{
    namespace event_system
    {
        struct WindowReizeEvent
        {
            s32 window_width;
            s32 window_height;
        };

        union Event
        {
            EventType type;
            WindowReizeEvent window_resize;
        };
    }
}
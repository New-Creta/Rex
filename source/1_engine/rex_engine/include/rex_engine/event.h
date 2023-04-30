#pragma once

#include "rex_engine/event_type.h"
#include "rex_engine/types.h"

namespace rex
{
  namespace event_system
  {
    struct WindowCommonEvent
    {
        EventType type;
    };

    struct WindowReizeEvent
    {
      EventType type;
      s32 window_width;
      s32 window_height;
    };

    union Event
    {
      EventType type;
      WindowCommonEvent window_common;
      WindowReizeEvent window_resize;
    };
  } // namespace event_system
} // namespace rex
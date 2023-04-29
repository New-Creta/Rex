#pragma once

#include "rex_engine/event_type.h"
#include "rex_engine/types.h"

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
  } // namespace event_system
} // namespace rex
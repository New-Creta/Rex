#pragma once

#include "rex_engine/event_type.h"
#include "rex_engine/types.h"

namespace rex
{
  namespace event_system
  {
    struct WindowResizeEvent
    {
      s32 window_width;
      s32 window_height;
    };

    struct Event
    {
      EventType type;

      union EventData
      {
        WindowResizeEvent window_resize;
      } data;
    };
  } // namespace event_system
} // namespace rex
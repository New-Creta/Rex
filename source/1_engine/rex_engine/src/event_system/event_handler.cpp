#include "rex_engine/event_system/event_handler.h"

namespace rex
{
  EventHandler& event_handler()
  {
    static EventHandler event_handler{};
    return event_handler;
  }
}
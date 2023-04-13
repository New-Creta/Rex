#pragma once

#include "rex_std/functional.h"

namespace rex
{
  namespace event_system
  {
    enum class EventType
    {
      CharDown,
      WindowClose
    };

    using EventFunction = rsl::function<void(void*)>;

    void subscribe(EventType type, const EventFunction& function);

    void fire_event(EventType type, void* userData);
  } // namespace event_system
} // namespace rex
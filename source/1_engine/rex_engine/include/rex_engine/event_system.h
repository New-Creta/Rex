#pragma once

#include "rex_std/functional.h"

namespace rex
{
  namespace event_system
  {
    enum class EventType
    {
      WindowClose,
      QuitApp
    };

    using EventFunction = rsl::function<void()>;

    void subscribe(EventType type, const EventFunction& function);

    void fire_event(EventType type);
  } // namespace event_system
} // namespace rex
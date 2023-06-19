#pragma once

#include "rex_engine/event.h" // IWYU pragma: keep
#include "rex_std/functional.h"

namespace rex
{
  namespace event_system
  {
    enum class EventType;

    using EventFunction = rsl::function<void(const Event&)>;

    void subscribe(EventType type, const EventFunction& function);

    void enqueue_event(const Event& evt);
    void fire_events();
  } // namespace event_system
} // namespace rex
#pragma once

#include "rex_std/functional.h" // for function
#include "rex_engine/event.h" // IWYU pragma: keep

namespace rex
{
  namespace event_system
  {
    enum class EventType;
    struct Event;

    using EventFunction = rsl::function<void(const Event&)>;

    void subscribe(EventType type, const EventFunction& function);

    void fire_event(const Event& evt);
  } // namespace event_system
} // namespace rex
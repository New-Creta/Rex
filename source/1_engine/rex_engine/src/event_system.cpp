#include "rex_engine/event_system.h"

#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

namespace rex
{
  rsl::unordered_map<event_system::EventType, rsl::vector<event_system::EventFunction>>& delegates_map()
  {
    static rsl::unordered_map<event_system::EventType, rsl::vector<event_system::EventFunction>> delegates_map;
    return delegates_map;
  }

  void event_system::subscribe(EventType type, const EventFunction& function)
  {
    delegates_map()[type].push_back(function);
  }

  void event_system::fire_event(EventType type)
  {
    const rsl::vector<EventFunction>& delegates = delegates_map()[type];

    for(const EventFunction& delegate: delegates)
    {
      delegate();
    }
  }
} // namespace rex
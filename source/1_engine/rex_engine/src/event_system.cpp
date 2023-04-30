#include "rex_engine/event_system.h"

#include "rex_engine/event_type.h"
#include "rex_engine/log.h"

#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"
#include "rex_std_extra/utility/enum_reflection.h"

// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)

namespace rex
{
  namespace event_system
  {
    rsl::unordered_map<EventType, rsl::vector<EventFunction>>& delegates_map()
    {
      static rsl::unordered_map<EventType, rsl::vector<EventFunction>> delegates_map;
      return delegates_map;
    }

    void subscribe(EventType type, const EventFunction& function)
    {
      delegates_map()[type].push_back(function);
    }

    void fire_event(const Event& evt)
    {
      switch (evt.type)
      {
      case EventType::WindowClose:
      case EventType::WindowActivate:
      case EventType::WindowDeactivate:
      case EventType::WindowStartWindowResize:
      case EventType::WindowStopWindowResize:
        REX_LOG(LogEngine, "Firing event: Event Type: {0}", rsl::enum_refl::enum_name(evt.type));
          break;
      case EventType::WindowMinimized:
      case EventType::WindowMaximized:
      case EventType::WindowRestored:
        REX_LOG(LogEngine, "Firing event: Event Type: {0} - Window Size: [{1}, {2}]", rsl::enum_refl::enum_name(evt.type), evt.window_resize.window_width, evt.window_resize.window_height);
          break;
      }

      const rsl::vector<EventFunction>& delegates = delegates_map()[evt.type];

      for(const EventFunction& delegate: delegates)
      {
        delegate(evt);
      }
    }
  } // namespace event_system
} // namespace rex

// NOLINTEND(cppcoreguidelines-pro-type-union-access)
#include "rex_engine/event_system.h"
#include "rex_engine/event_type.h"

#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

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
            const rsl::vector<EventFunction>& delegates = delegates_map()[evt.type];

            for (const EventFunction& delegate : delegates)
            {
                delegate(evt);
            }
        }
    }
} // namespace rex
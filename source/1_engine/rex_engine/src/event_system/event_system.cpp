#include "rex_engine/event_system/event_system.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/event_system/event.h"
#include "rex_std/bonus/utility/enum_reflection.h"
#include "rex_std/mutex.h"
#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)


namespace rex
{
  EventSystem& event_system()
  {
    static EventSystem event_system{};
    return event_system;
  }

  EventSystem::EventSystem()
    : m_event_queue(s_event_queue_byte_size)
    , m_num_events_queued(0)
  {}

  void EventSystem::remove_subscription(SubscriptionHandle handle)
  {
    EventDispatcherBase* dispatcher = m_dispatchers.at(handle.type_id()).get();
    dispatcher->remove_function(handle.id());
  }

  void EventSystem::dispatch_queued_events()
  {
    while (m_num_events_queued > 0)
    {
      // Read the event type info first, this will give us information how much we actually should read
      rsl::aligned_storage_t<EventBase> event_base_storage;
      m_event_queue.peek(&event_base_storage, sizeof(event_base_storage));
      EventBase event_base = *event_base_storage.get<EventBase>();

      // If we have a dispatcher for this event, call it
      if (m_dispatchers.contains(event_base.type_id()))
      {
        EventDispatcherBase* dispatcher = m_dispatchers.at(event_base.type_id()).get();
        s32 event_size = static_cast<s32>(dispatcher->event_size());
        REX_ASSERT_X(event_size == event_base.event_size(), "Mismatching event size in dispatcher vs base type");
        m_intermediate_event_data.resize(event_size);
        m_event_queue.read(m_intermediate_event_data.data(), event_size);
        dispatcher->dispatch_from_data(m_intermediate_event_data.data(), event_size);
      }
      // otherwise increment the read offset in the event queue with the total size of this event
      else
      {
        m_event_queue.skip(static_cast<s32>(event_base.event_size()));
      }
      --m_num_events_queued;
    }
  }
} // namespace rex

  // NOLINTEND(cppcoreguidelines-pro-type-union-access)
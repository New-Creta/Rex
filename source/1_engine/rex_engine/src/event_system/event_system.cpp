#include "rex_engine/event_system/event_system.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/event_system/event.h"
#include "rex_std/bonus/utility/enum_reflection.h"
#include "rex_std/mutex.h"
#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)

#include "rex_engine/diagnostics/log.h"

namespace rex
{
  DEFINE_LOG_CATEGORY(LogEventSystem);

  EventSystem& event_system()
  {
    static EventSystem event_system{};
    return event_system;
  }

  EventSystem::EventSystem()
    : m_event_queue(s_event_queue_byte_size)
    , m_num_events_queued(0)
  {}

  void EventSystem::dispatch_queued_events()
  {
    while (m_num_events_queued > 0)
    {
      // Read the event type info first, this will give us information how much we actually should read
      rsl::aligned_storage_t<rsl::type_id_t> type_id;
      m_event_queue.peek(&type_id, sizeof(type_id));
      rsl::type_id_t event_type_id = *type_id.get<rsl::type_id_t>();

      REX_INFO(LogEventSystem, "Processing: {}", event_type_id.name());

      if (!m_dispatchers.contains(event_type_id))
      {
        return;
      }

      // If we have a dispatcher for this event, call it
      EventDispatcherBase* dispatcher = m_dispatchers.at(event_type_id).get();
      s32 event_size = static_cast<s32>(dispatcher->event_size());
      m_intermediate_event_data.resize(event_size);
      m_event_queue.read(m_intermediate_event_data.data(), event_size);
      dispatcher->dispatch_from_data(m_intermediate_event_data.data(), event_size);

      --m_num_events_queued;
    }
  }
} // namespace rex

  // NOLINTEND(cppcoreguidelines-pro-type-union-access)
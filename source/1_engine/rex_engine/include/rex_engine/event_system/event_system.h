#pragma once

#include "rex_engine/event_system/event.h" // IWYU pragma: keep
#include "rex_std/functional.h"
#include "rex_std/unordered_map.h"
#include "rex_std/bonus/utility.h"

#include "rex_engine/containers/typeless_ring_buffer.h"
#include "rex_engine/event_system/event_dispatcher.h"

#include "rex_engine/diagnostics/log.h"

namespace rex
{
  class EventSystem
  {
  public:
    // constructs the event system.
    // mostly allocates memory in the event queue for the events
    EventSystem();

    // subscribe to a certain event being fired
    template <typename Event>
    void subscribe(const EventDispatcherFunc<Event>& eventFunc)
    {
      static_assert(rsl::is_base_of_v<EventBase, Event>, "Invalid event type. T does not derive from EventBase class ");
      EventDispatcher<Event>* event_dispatcher = dispatcher<Event>();
      event_dispatcher->add_function(eventFunc);
    }

    // enqueue the event in a pool, to be processed at the next frame
    template <typename Event>
    void enqueue_event(const Event& ev)
    {
      static_assert(rsl::is_base_of_v<EventBase, Event>, "Invalid event type. T does not derive from EventBase class ");
      m_num_events_queued++;
      m_event_queue.write(&ev, sizeof(ev));
    }

    // fire off an event to be handled immediately.
    template <typename Event>
    void fire_event(const Event& ev)
    {
      static_assert(rsl::is_base_of_v<EventBase, Event>, "Invalid event type. T does not derive from EventBase class ");
      EventDispatcher<Event>* event_dispatcher = dispatcher<Event>();
      event_dispatcher->dispatch(ev);
    }

    // dispatch the event that were queued last frame
    void dispatch_queued_events();

  private:
    template <typename T>
    EventDispatcher<T>* dispatcher()
    {
      rsl::type_id_t type_id = rsl::type_id<T>();
      if (!m_dispatchers.contains(type_id))
      {
        m_dispatchers.emplace(type_id, rsl::make_unique<EventDispatcher<T>>());
      }

      return static_cast<EventDispatcher<T>*>(m_dispatchers.at(type_id).get());
    }

  private:
    rsl::unordered_map<rsl::type_id_t, rsl::unique_ptr<EventDispatcherBase>> m_dispatchers;
    TypelessRingBuffer m_event_queue;
    rsl::vector<rsl::byte> m_intermediate_event_data;
    static constexpr rsl::memory_size s_event_queue_byte_size = 256;
    s32 m_num_events_queued; // flag to indicate events were queued this frame
  };
  EventSystem& event_system();
} // namespace rex
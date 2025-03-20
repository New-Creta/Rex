#pragma once

#include "rex_engine/event_system/event.h" // IWYU pragma: keep
#include "rex_std/functional.h"
#include "rex_std/unordered_map.h"
#include "rex_std/bonus/utility.h"

#include "rex_engine/event_system/event_dispatcher.h"

#include "rex_engine/diagnostics/log.h"

#include "rex_engine/memory/allocators/stack_allocator.h"

namespace rex
{
  // #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: EVENT SYSTEM

  // A subscription handle is a handle the user could optional keep
  // if they ever want to remove a subscription from the event system at a future time
  class SubscriptionHandle
  {
  public:
    SubscriptionHandle(rsl::type_id_t typeId, s32 id)
      : m_type_id(typeId)
      , m_id(id)
    {}

    // the id of this subscription
    s32 id() const
    {
      return m_id;
    }

    // the type id of the event for this subscription
    rsl::type_id_t type_id() const
    {
      return m_type_id;
    }

  private:
    rsl::type_id_t m_type_id;
    s32 m_id;
  };

  class EventSystem
  {
  public:
    // constructs the event system.
    // mostly allocates memory in the event queue for the events
    EventSystem();

    // subscribe to a certain event being fired
    template <typename Event>
    SubscriptionHandle subscribe(const rsl::function<void(const Event&)>& eventFunc)
    {
      static_assert(rsl::is_base_of_v<EventBase, Event>, "Invalid event type. T does not derive from EventBase class ");
      EventDispatcher<Event>* event_dispatcher = dispatcher<Event>();
      s32 func_id = event_dispatcher->add_function(eventFunc);
      return SubscriptionHandle(rsl::type_id<Event>(), func_id);
    }

    // Remove a previously bound subscription from the list
    void remove_subscription(SubscriptionHandle handle);

    // enqueue the event in a pool, to be processed at the next frame
    template <typename Event>
    void enqueue_event(const Event& ev)
    {
      static_assert(rsl::is_base_of_v<EventBase, Event>, "Invalid event type. T does not derive from EventBase class ");
      REX_ASSERT_X(m_event_allocator.can_allocate(sizeof(ev)), "Writting too many events in 1 frame, event buffer is full. Num queued events: {}", m_enqueued_events.size());

      // Allocate space for the new event
      Event* newEvent = m_event_allocator.allocate<Event>();
      rsl::memcpy(newEvent, rsl::addressof(ev), sizeof(ev));

      // Push the new event to the list
      m_enqueued_events.push_back(newEvent);
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
    // return the dispatcher for the event
    // create a new one if one doesn't already exist
    template <typename Event>
    EventDispatcher<Event>* dispatcher()
    {
      rsl::type_id_t type_id = rsl::type_id<Event>();
      if (!m_dispatchers.contains(type_id))
      {
        m_dispatchers.emplace(type_id, rsl::make_unique<EventDispatcher<Event>>());
      }

      return static_cast<EventDispatcher<Event>*>(m_dispatchers.at(type_id).get());
    }

  private:
    rsl::unordered_map<rsl::type_id_t, rsl::unique_ptr<EventDispatcherBase>> m_dispatchers;
    StackAllocator m_event_allocator;
    rsl::vector<EventBase*> m_enqueued_events;
    static constexpr rsl::memory_size s_event_queue_byte_size = 256_kib;
  };
  EventSystem& event_system();
} // namespace rex
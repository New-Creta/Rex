#pragma once

#include "rex_engine/event_system/event_dispatcher.h"
#include "rex_engine/event_system/event.h"

#include "rex_std/unordered_map.h"
#include "rex_std/bonus/utility.h"

namespace rex
{
  class EventHandler
  {
  public:
    template <typename T>
    void add_callback(const EventDispatcherFunc<T>& eventFunc)
    {
      static_assert(rsl::is_base_of_v<EventBase, T>, "Invalid event for event handler. T does not derive from EventBase class ");
      EventDispatcher<T>* event_dispatcher = dispatcher<T>();
      event_dispatcher->add_function(eventFunc);
    }

    template <typename T>
    void handle_event(const T& ev)
    {
      static_assert(rsl::is_base_of_v<EventBase, T>, "Invalid event for event handler. T does not derive from EventBase class ");
      EventDispatcher<T>* event_dispatcher = dispatcher<T>();
      event_dispatcher->dispatch(ev);
    }

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
  };

  EventHandler& event_handler();
}
#pragma once

#include "rex_std/functional.h"
#include "rex_std/type_traits.h"
#include "rex_std/vector.h"
#include "rex_std/bonus/memory.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  template <typename T>
  class EventDispatcherFunc
  {
  public:
    EventDispatcherFunc(s32 id, const rsl::function<void(const T&)>& func)
      : m_function(func)
      , m_id(id)
    {

    }

    s32 id() const
    {
      return m_id;
    }

    void operator()(const T& ev) const
    {
      m_function(ev);
    }

  private:
    rsl::function<void(const T&)> m_function;
    s32 m_id;
  };

  class EventDispatcherBase
  {
  public:
    EventDispatcherBase(rsl::memory_size eventSize)
      : m_event_size(eventSize)
    {}

    EventDispatcherBase(const EventDispatcherBase&) = delete;
    EventDispatcherBase(EventDispatcherBase&&) = delete;
    virtual ~EventDispatcherBase() = default;

    EventDispatcherBase& operator=(const EventDispatcherBase&) = delete;
    EventDispatcherBase& operator=(EventDispatcherBase&&) = delete;

    rsl::memory_size event_size() const
    {
      return m_event_size;
    }

    virtual void dispatch_from_data(void* data, rsl::memory_size size) = 0;
    virtual void remove_function(s32 id) = 0;

  private:
    rsl::memory_size m_event_size;
  };

  template <typename EventType>
  class EventDispatcher : public EventDispatcherBase
  {
    static_assert(rsl::is_base_of_v<EventBase, EventType>, "Invalid event type for dispatcher, T not derived from Event class");
    using event_type = EventType;

  public:
    EventDispatcher()
      : EventDispatcherBase(sizeof(EventType))
    {}

    void dispatch(const EventBase& event)
    {
      const event_type& ev = static_cast<const event_type&>(event);
      for (const EventDispatcherFunc<EventType>& function : m_functions)
      {
        function(ev);
      }
    }

    void dispatch_from_data(void* data, rsl::memory_size size) override
    {
      REX_ASSERT_X(size == sizeof(EventType), "Trying to dispatch an event from data but size doesn't match");
      EventType* ev = reinterpret_cast<EventType*>(data);
      dispatch(*ev);
    }

    s32 add_function(const rsl::function<void(const EventType&)>& func)
    {
      m_functions.emplace_back(++m_internal_id_counter, func);
      return m_internal_id_counter;
    }

    void remove_function(s32 id) override
    {
      m_functions.erase(rsl::remove_if(m_functions.begin(), m_functions.end(),
        [id](const EventDispatcherFunc<EventType>& func)
        {
          return func.id() == id;
        }));
    }
  private:
    rsl::vector<EventDispatcherFunc<EventType>> m_functions;
    s32 m_internal_id_counter;
  };
}
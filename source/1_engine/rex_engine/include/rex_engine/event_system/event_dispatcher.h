#pragma once

#include "rex_std/functional.h"
#include "rex_std/type_traits.h"
#include "rex_std/vector.h"

namespace rex
{
  template <typename T>
  using EventDispatcherFunc = rsl::function<void(const T&)>;

  class EventDispatcherBase
  {
  public:
    virtual ~EventDispatcherBase() = default;
  };

  template <typename T>
  class EventDispatcher : public EventDispatcherBase
  {
    static_assert(rsl::is_base_of_v<EventBase, T>, "Invalid event type for dispatcher, T not derived from Event class");

  public:
    void dispatch(const T& event)
    {
      for (const EventDispatcherFunc<T>& function : m_functions)
      {
        function(event);
      }
    }

    void add_function(const EventDispatcherFunc<T>& func)
    {
      m_functions.push_back(func);
    }
  private:
    rsl::vector<EventDispatcherFunc<T>> m_functions;
  };
}
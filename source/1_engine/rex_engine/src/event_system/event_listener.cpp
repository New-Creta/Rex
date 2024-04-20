//#include "rex_engine/event_system/event_listener.h"
//
//#include "rex_engine/event_system/event_handler.h"
//#include "rex_engine/event_system/event_dispatcher.h"
//#include "rex_engine/event_system/event.h"
//
//#include "rex_std/type_traits.h"
//
//namespace rex
//{
//  class EventListener
//  {
//  public:
//    template <typename T>
//    void subscribe_on_event(const EventDispatcherFunc<T>& eventFunc)
//    {
//      static_assert(rsl::is_base_of_v<Event, T>, "T is not an event type");
//
//      m_event_handler.add_function<T>(eventFunc);
//    }
//
//
//
//  private:
//    EventHandler m_event_handler;
//  };
//}
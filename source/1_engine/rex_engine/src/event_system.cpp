#include "rex_engine/event_system.h"

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/event.h"
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
    namespace internal
    {
      class EventQueue
      {
      public:
        EventQueue()
            :m_subscriptions()
            ,m_front()
            ,m_back()
            ,m_current(&m_front)
            ,m_next(&m_back)
        {
        }

        void subscribe(EventType type, const EventFunction& function)
        {
          m_subscriptions[type].push_back(function);
        }

        void enqueue(const Event& evt)
        {
          m_next->push_back(evt);
        }

        void fire()
        {
          for(auto& evt: *m_current)
          {
            const rsl::vector<EventFunction>& delegates = m_subscriptions[evt.type];

            for(const EventFunction& delegate: delegates)
            {
              delegate(evt);
            }
          }

          m_current->clear();
        }

        void present()
        {
          rsl::vector<Event>* temp = m_current;
          m_current                = m_next;
          m_next                   = temp;
        }

      private:
        rsl::unordered_map<EventType, rsl::vector<EventFunction>> m_subscriptions;

        rsl::vector<Event> m_front;
        rsl::vector<Event> m_back;

        rsl::vector<Event>* m_current;
        rsl::vector<Event>* m_next;
      };
    } // namespace internal

    internal::EventQueue& event_queue()
    {
      static internal::EventQueue queue;
      return queue;
    }

    void subscribe(EventType type, const EventFunction& function)
    {
      event_queue().subscribe(type, function);
    }

    void enqueue_event(const Event& evt)
    {
      switch(evt.type)
      {
        case EventType::WindowClose:
        case EventType::WindowActivate:
        case EventType::WindowDeactivate:
        case EventType::WindowStartWindowResize:
        case EventType::QuitApp:
        case EventType::WindowStopWindowResize: REX_LOG(LogEngine, "Firing event: Event Type: {0}", rsl::enum_refl::enum_name(evt.type)); break;
        case EventType::WindowMinimized:
        case EventType::WindowMaximized:
        case EventType::WindowRestored: REX_LOG(LogEngine, "Firing event: Event Type: {0} - Window Size: [{1}, {2}]", rsl::enum_refl::enum_name(evt.type), evt.data.window_resize.window_width, evt.data.window_resize.window_height); break;
      }

      event_queue().enqueue(evt);
    }

    void fire_events()
    {
      event_queue().fire();
      event_queue().present();
    }
  } // namespace event_system
} // namespace rex

// NOLINTEND(cppcoreguidelines-pro-type-union-access)
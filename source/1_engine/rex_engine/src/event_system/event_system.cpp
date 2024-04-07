#include "rex_engine/event_system/event_system.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/event_system/event.h"
#include "rex_engine/event_system/event_type.h"
#include "rex_std/bonus/utility/enum_reflection.h"
#include "rex_std/mutex.h"
#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

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
            : m_subscriptions()
            , m_front()
            , m_back()
            , m_current(&m_front)
            , m_next(&m_back)
        {
        }

        void subscribe(EventType type, const EventFunction& function)
        {
          m_subscriptions[type].push_back(function);
        }

        void enqueue(const Event& evt)
        {
          const rsl::unique_lock lock(m_enqueue_mtx);
          m_next->push_back(evt);
        }

        void process()
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
          const rsl::unique_lock lock(m_enqueue_mtx);
          rsl::swap(m_current, m_next);
        }

      private:
        rsl::unordered_map<EventType, rsl::vector<EventFunction>> m_subscriptions;

        rsl::vector<Event> m_front;
        rsl::vector<Event> m_back;

        rsl::vector<Event>* m_current;
        rsl::vector<Event>* m_next;

        rsl::mutex m_enqueue_mtx;
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
        case EventType::WindowStopWindowResize: REX_INFO(LogEngine, "Firing event: Event Type: {0}", rsl::enum_refl::enum_name(evt.type)); break;
        case EventType::WindowMinimized:
        case EventType::WindowMaximized:
        case EventType::WindowRestored: REX_INFO(LogEngine, "Firing event: Event Type: {0} - Window Size: [{1}, {2}]", rsl::enum_refl::enum_name(evt.type), evt.data.window_resize.window_width, evt.data.window_resize.window_height); break;
      }

      event_queue().enqueue(evt);
    }

    void enqueue_event(EventType evt)
    {
      const Event event {evt};
      enqueue_event(event);
    }

    void process_events()
    {
      // Swap the buffers first so next becomes current and vice versa
      event_queue().present();

      // Then process all the events that are now in the queue
      event_queue().process();
    }

  } // namespace event_system
} // namespace rex

  // NOLINTEND(cppcoreguidelines-pro-type-union-access)
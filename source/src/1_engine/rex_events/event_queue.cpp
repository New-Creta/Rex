#include "rex_events_pch.h"

#include "event_queue.h"

namespace rex
{
    namespace events
    {
        //-------------------------------------------------------------------------
        EventQueue::EventQueue()
            : m_is_active(false)
        {
        }

        //-------------------------------------------------------------------------
        EventQueue::~EventQueue() = default;

        //-------------------------------------------------------------------------
        void EventQueue::activate()
        {
            m_is_active = true;
        }

        //-------------------------------------------------------------------------
        void EventQueue::deactivate()
        {
            m_is_active = true;
        }

        //-------------------------------------------------------------------------
        bool EventQueue::enqueue(Event& evt)
        {
            return m_queue.push_back(evt);
        }

        //-------------------------------------------------------------------------
        Event EventQueue::next()
        {
            if (empty())
            {
                return nullptr;
            }

            Event e;
            if (!m_queue.pop_front(e))
            {
                return nullptr;
            }

            return e;
        }

        //-------------------------------------------------------------------------
        bool EventQueue::active() const
        {
            return m_is_active;
        }

        //-------------------------------------------------------------------------
        bool EventQueue::empty() const
        {
            return m_queue.is_empty();
        }
    }
}
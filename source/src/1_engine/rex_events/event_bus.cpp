#include "rex_events_pch.h"

#include "event_bus.h"

namespace rex
{
    namespace events
    {
        //-------------------------------------------------------------------------
        EventBusImpl::EventBusImpl()
            : m_active_queue(nullptr)
        {
        }

        //-------------------------------------------------------------------------
        EventBusImpl::~EventBusImpl()
        {
        }

        //-------------------------------------------------------------------------
        rex::events::EventQueue* EventBusImpl::get_active_queue() const
        {
            return m_active_queue;
        }

        //-------------------------------------------------------------------------
        size_t EventBusImpl::push_event_queue(EventQueue* queue, Activate activate)
        {
            m_queues.push_back(queue);

            if (activate)
            {
                m_active_queue = queue;
                m_active_queue->activate();
            }

            return queue->get_instance_id();
        }

        //-------------------------------------------------------------------------
        void EventBusImpl::activate_event_queue(size_t queueID)
        {
            auto it = std::find_if(std::begin(m_queues), std::end(m_queues), [queueID](const EventQueue* queue)
                                   {
                                       return queue->get_instance_id() == queueID;
                                   });

            if (it != std::cend(m_queues))
            {
                if (m_active_queue != nullptr)
                {
                    m_active_queue->deactivate();
                }

                m_active_queue = (*it);
                m_active_queue->activate();
            }
        }

        //-------------------------------------------------------------------------
        void EventBusImpl::deactivate_event_queue(size_t queueID)
        {
            auto it = std::find_if(std::begin(m_queues), std::end(m_queues), [queueID](const EventQueue* queue)
                                   {
                                       return queue->get_instance_id() == queueID;
                                   });

            if (it != std::cend(m_queues))
            {
                (*it)->deactivate();

                if (m_active_queue == (*it))
                {
                    m_active_queue = nullptr;
                }
            }
        }

        //-------------------------------------------------------------------------
        void EventBusImpl::disable_all()
        {
            for (EventQueue* queue : m_queues)
            {
                queue->deactivate();
            }

            m_active_queue = nullptr;
        }
    }
}
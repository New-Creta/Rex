#pragma once

#include "containers/threading/safe_ring_buffer.h"

#include "event.h"
#include "patterns/instance_counter.h"

namespace rex
{
    namespace events
    {
        class EventQueue : public InstanceCounter<EventQueue>
        {
        public:
            static const int EVENT_QUEUE_PUMP_COUNT = 256;
            static const int EVENT_QUEUE_CHANNELS = 256;

            EventQueue();
            ~EventQueue();

            void activate();
            void deactivate();

            bool enqueue(Event& evt);

            Event next();

            bool active() const;
            bool empty() const;

        private:
            safe_ringbuffer<Event, EVENT_QUEUE_CHANNELS> m_queue;

            bool m_is_active;
        };
    }
}
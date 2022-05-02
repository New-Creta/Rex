#pragma once

#include "patterns/singleton.h"

#include "event_queue.h"

namespace rex
{
    namespace events
    {
        class EventBusImpl
        {
        public:
            using Activate = rtl::YesNoEnum;

            EventBusImpl();
            ~EventBusImpl();

            EventQueue* get_active_queue() const;

            size_t push_event_queue(EventQueue* queue, Activate activate = Activate::No);

            void activate_event_queue(size_t queueID);
            void deactivate_event_queue(size_t queueID);

            void disable_all();

        private:
            std::vector<EventQueue*> m_queues;
            EventQueue* m_active_queue;
        };

        class EventBus : public StaticSingleton<EventBusImpl>
        {
        };
    }
}
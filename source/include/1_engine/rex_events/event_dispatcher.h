#pragma once

#include "event.h"

namespace rex
{
    namespace events
    {
        class EventDispatcher
        {
        public:
            EventDispatcher(Event& event);

            template <typename T, typename F>
            bool dispatch(F func);

        private:
            Event& m_event;
        };

        //-------------------------------------------------------------------------
        template <typename T, typename F>
        bool EventDispatcher::dispatch(F func)
        {
            if (m_event->get_type() == T::get_static_type() && !is_consumed(m_event))
            {
                if (func(static_cast<T&>(*m_event)))
                {
                    m_event->handle();
                }

                return true;
            }

            return false;
        }
    }
}
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

            template<typename T, typename F>
            bool dispatch(F func);

        private:
            const Event& m_event;
        };

        //-------------------------------------------------------------------------
        template<typename T, typename F>
        typename bool rex::events::EventDispatcher::dispatch(F func)
        {
            if (m_event.getType() == T::getStaticType())
            {
                if (func(static_cast<const T&>(m_event)))
                    m_event.handle();

                return true;
            }

            return false;
        }
    }
}
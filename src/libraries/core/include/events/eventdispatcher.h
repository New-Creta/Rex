#pragma once

#include "rex_core_global.h"

#include "event.h"

namespace rex
{
    namespace events
    {
        class EventDispatcher
        {
        public:
            REX_CORE_EXPORT EventDispatcher(Event& event);

            template<typename T, typename F>
            bool dispatch(F func);

        private:
            Event& m_event;
        };

        //-------------------------------------------------------------------------
        template<typename T, typename F>
        typename bool rex::events::EventDispatcher::dispatch(F func)
        {
            if (m_event.getType() == T::getStaticType())
            {
                if (func(static_cast<T&>(m_event)))
                    m_event.handle();

                return true;
            }

            return false;
        }
    }
}
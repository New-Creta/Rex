#include "rex_events_pch.h"
#include "event_dispatcher.h"

namespace rex
{
    namespace events
    {
        //-------------------------------------------------------------------------
        EventDispatcher::EventDispatcher(Event& event)
            : m_event(event)
        {
        }
    }
}
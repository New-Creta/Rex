#include "rex_events_pch.h"
#include "event.h"

namespace rex
{
    namespace events
    {
        //-------------------------------------------------------------------------
        BaseEvent::BaseEvent(/* const memory::Blob& nativeEvent */)
            : m_handled(false)
            //, m_native_event(nativeEvent)
        {
        }
        //-------------------------------------------------------------------------
        BaseEvent::~BaseEvent() = default;

        //-------------------------------------------------------------------------
        void BaseEvent::handle()
        {
            m_handled = true;
        }

        //-------------------------------------------------------------------------
        std::string BaseEvent::to_string() const
        {
            return typeid(this).name();
        }

        //-------------------------------------------------------------------------
        bool BaseEvent::is_in_category(EventCategory category) const
        {
            return get_category() & category;
        }
        //-------------------------------------------------------------------------
        bool BaseEvent::is_handled() const
        {
            return m_handled;
        }

        // //-------------------------------------------------------------------------
        // const rex::memory::Blob& BaseEvent::get_native_event() const
        // {
        //     return m_native_event;
        // }

        //-------------------------------------------------------------------------
        std::ostream& operator<<(std::ostream& os, const std::shared_ptr<BaseEvent>& e)
        {
            return os << e->to_string();
        }
    }
}
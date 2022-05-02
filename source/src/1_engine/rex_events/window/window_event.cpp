#include "rex_events_pch.h"
#include "window/window_event.h"

namespace rex
{
    namespace events
    {
        //-------------------------------------------------------------------------
        WindowEvent::WindowEvent(uint32 windowID/* , const memory::Blob& nativeEvent */)
            : BaseEvent(/* nativeEvent */)
            , m_window_id(windowID)
        {
        }
        //-------------------------------------------------------------------------
        WindowEvent::~WindowEvent() = default;

        //-------------------------------------------------------------------------
        uint32 WindowEvent::get_window_id() const
        {
            return m_window_id;
        }
    }
}
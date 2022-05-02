#include "rex_events_pch.h"
#include "window/window_close.h"

namespace rex
{
    namespace events
    {
        //-------------------------------------------------------------------------
        WindowClose::WindowClose(uint32 windowID/* , const memory::Blob& nativeEvent */)
            : WindowEvent(windowID/* , nativeEvent */)
        {
            // R_TRACE("Window close event created");
        }
        //-------------------------------------------------------------------------
        WindowClose::~WindowClose() = default;

        //-------------------------------------------------------------------------
        bool WindowClose::can_consume() const
        {
            return true;
        }

        //-------------------------------------------------------------------------
        std::string WindowClose::to_string() const
        {
            std::stringstream s;
            s << "Window Closed";
            return s.str();
        }
    }
}
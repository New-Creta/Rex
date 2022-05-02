#include "rex_events_pch.h"
#include "window/window_show.h"

namespace rex
{
    namespace events
    {
        //-------------------------------------------------------------------------
        WindowShow::WindowShow(uint32 windowID/* , const memory::Blob& nativeEvent */)
            : WindowEvent(windowID/* , nativeEvent */)
        {
            // R_TRACE("Window show event created");
        }
        //-------------------------------------------------------------------------
        WindowShow::~WindowShow()
        {
        }

        //-------------------------------------------------------------------------
        bool WindowShow::can_consume() const
        {
            return true;
        }

        //-------------------------------------------------------------------------
        std::string WindowShow::to_string() const
        {
            std::stringstream s;
            s << "Window Shown";
            return s.str();
        }
    }
}
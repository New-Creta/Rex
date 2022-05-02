#include "rex_events_pch.h"
#include "window/window_hide.h"

namespace rex
{
    namespace events
    {
        //-------------------------------------------------------------------------
        WindowHide::WindowHide(uint32 windowID/* , const memory::Blob& nativeEvent */)
            : WindowEvent(windowID/* , nativeEvent */)
        {
            // R_TRACE("Window hide event created");
        }
        //-------------------------------------------------------------------------
        WindowHide::~WindowHide()
        {
        }

        //-------------------------------------------------------------------------
        bool WindowHide::can_consume() const
        {
            return false;
        }

        //-------------------------------------------------------------------------
        std::string WindowHide::to_string() const
        {
            std::stringstream s;
            s << "Window Hide";
            return s.str();
        }
    }
}
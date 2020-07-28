#include "events/window/WindowActivated.h"

//-------------------------------------------------------------------------
rex::events::WindowActivated::WindowActivated(rex::Window* window)
    : WindowEvent(window)
{
    RX_TRACE("Window activate event created");
}
//-------------------------------------------------------------------------
rex::events::WindowActivated::~WindowActivated() = default;
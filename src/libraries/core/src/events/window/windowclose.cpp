#include "events/window/WindowClose.h"

//-------------------------------------------------------------------------
rex::events::WindowClose::WindowClose(rex::Window* window)
    : WindowEvent(window)
{
    RX_TRACE("Window close event created");
}
//-------------------------------------------------------------------------
rex::events::WindowClose::~WindowClose() = default;
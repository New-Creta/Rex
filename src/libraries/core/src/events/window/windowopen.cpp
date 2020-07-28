#include "events/window/WindowOpen.h"

//-------------------------------------------------------------------------
rex::events::WindowOpen::WindowOpen(rex::Window* window)
    : WindowEvent(window)
{
    RX_TRACE("Window open event created");
}
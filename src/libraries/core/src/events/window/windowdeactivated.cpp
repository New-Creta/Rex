#include "events/window/WindowDeactivated.h"

//-------------------------------------------------------------------------
rex::events::WindowDeactivated::WindowDeactivated(rex::Window* window)
	: WindowEvent(window)
{
	RX_TRACE("Window deactivated event created");
}

//-------------------------------------------------------------------------
rex::events::WindowDeactivated::~WindowDeactivated() = default;
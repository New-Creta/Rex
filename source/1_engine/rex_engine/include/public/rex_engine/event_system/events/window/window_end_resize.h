#pragma once

#include "rex_engine/event_system/event.h"

namespace rex
{
	// Doesn't hold any members, just used to indicate the window resizing has ended
	// this event gets raised after a resize has finished
	class WindowEndResize : public Event<WindowEndResize>
	{
	};
}
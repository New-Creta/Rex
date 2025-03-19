#pragma once

#include "rex_engine/event_system/event.h"

namespace rex
{
	// Doesn't hold any members, just used to indicate the window will start resizing
	class WindowStartResize : public Event<WindowStartResize>
	{
	};
}
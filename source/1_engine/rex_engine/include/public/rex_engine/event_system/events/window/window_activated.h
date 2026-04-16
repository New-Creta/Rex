#pragma once

#include "rex_engine/event_system/event.h"

namespace rex
{
	// Doesn't hold any members, just used to indicate the window will be activated
	// this event is raised when the window is activated/gets into focus
	// for example when the user selects the window when it was not in focus
	class WindowActivated : public Event<WindowActivated>
	{
	};
}
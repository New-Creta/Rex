#pragma once

#include "rex_engine/event_system/event.h"

namespace rex
{
	// Doesn't hold any members, just used to indicate the window will be deactivated
	// This event gets raised when the window loses focus
	// for example when the users ALT + TAB to another window
	class WindowDeactivated : public Event<WindowDeactivated>
	{
	};
}
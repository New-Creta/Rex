#pragma once

#include "rex_engine/event_system/event.h"

namespace rex
{
	// Doesn't hold any members, only used to indicate that the window will be closed
	class WindowClose : public Event<WindowClose>
	{
	};
}
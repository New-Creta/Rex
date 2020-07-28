#pragma once

#include "rex_core_global.h"

#include "events/window/windowevent.h"

namespace rex
{
	namespace events
	{
		class WindowDeactivated : public WindowEvent
		{
		public:
			EVENT_CLASS_TYPE(EventType::WINDOW_DEACTIVATED);

			REX_CORE_EXPORT WindowDeactivated(rex::Window* window);
			REX_CORE_EXPORT ~WindowDeactivated() final;
		};
	}
}

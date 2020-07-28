#pragma once

#include "rex_core_global.h"

#include "events/window/windowevent.h"

namespace rex
{
	class Window;

	namespace events
	{
		class WindowClose : public WindowEvent
		{
		public:
			EVENT_CLASS_TYPE(EventType::WINDOW_CLOSED);

			REX_CORE_EXPORT WindowClose(rex::Window* window);
			REX_CORE_EXPORT ~WindowClose() final;
		};
	}
}

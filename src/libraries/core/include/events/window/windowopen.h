#pragma once

#include "rex_core_global.h"

#include "events/window/windowevent.h"
#include "events/genericevent.h"

namespace rex
{
	class Window;

	namespace events
	{
		class WindowOpen : public WindowEvent, public GenericEvent
		{
		public:
			EVENT_CLASS_TYPE(EventType::WINDOW_OPEN);

			REX_CORE_EXPORT WindowOpen(rex::Window* window);

			//-------------------------------------------------------------------------
			inline const NativeEvent* getNativeEvent() const final { return this; }
		};
	}
}
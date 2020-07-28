#pragma once

#include "rex_core_global.h"

#include "events/input/mouseevent.h"

namespace rex
{
	namespace events
	{
		class MouseMove : public MouseEvent
		{
		public:
			EVENT_CLASS_TYPE(EventType::MOUSE_MOVED);

			REX_CORE_EXPORT MouseMove(rex::Window* window, const MouseKeyMetaData& keyMetaData, const MousePosition& mousePos);
			REX_CORE_EXPORT ~MouseMove() override;
		};
	}
}
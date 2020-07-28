#pragma once

#include "rex_core_global.h"

#include "core/mousecodes.h"

#include "events/input/mouseevent.h"

namespace rex
{
	namespace events
	{
		class MouseUp : public MouseEvent
		{
		public:
			EVENT_CLASS_TYPE(EventType::MOUSE_BUTTON_RELEASED);

			REX_CORE_EXPORT MouseUp(rex::Window* window, const MouseKeyMetaData& keyMetaData, MouseCode button, const MousePosition& mousePos);
			REX_CORE_EXPORT ~MouseUp();

			REX_CORE_EXPORT MouseCode getButton() const;

		private:
			MouseCode m_button;
		};
	}
}
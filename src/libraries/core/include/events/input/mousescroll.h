#pragma once

#include "rex_core_global.h"

#include "events/input/mouseevent.h"
#include "events/input/mousescrollmeta.h"

namespace rex
{
	namespace events
	{
		class MouseScroll : public MouseEvent
		{
		public:
			EVENT_CLASS_TYPE(EventType::MOUSE_SCROLLED);

			REX_CORE_EXPORT MouseScroll(rex::Window* window, const MouseKeyMetaData& keyMetaData, ScrollMetaData scrollMetaData, const MousePosition& mousePos);
			REX_CORE_EXPORT ~MouseScroll() override;

			REX_CORE_EXPORT ScrollDirection getScrollDirection() const;
			REX_CORE_EXPORT int32 getScrollDelta() const;

		private:
			ScrollDirection m_direction;
			int32 m_wheel_delta;
		};
	}
}
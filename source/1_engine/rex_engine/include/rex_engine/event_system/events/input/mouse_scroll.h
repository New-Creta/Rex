#pragma once

#include "rex_engine/event_system/events/input/mouse_event.h"
#include "rex_engine/input/mouse_position.h"

namespace rex
{
	class MouseScroll : public MouseEvent
	{
	public:
		MouseScroll(int32 wheelDelta, MousePosition position)
			: m_wheel_delta(wheelDelta)
			, m_position(position)
		{}

		int32 wheel_delta() const
		{
			return m_wheel_delta;
		}
		MousePosition mouse_position() const
		{
			return m_position;
		}

	private:
		int32 m_wheel_delta;
		MousePosition m_position;
	};
}
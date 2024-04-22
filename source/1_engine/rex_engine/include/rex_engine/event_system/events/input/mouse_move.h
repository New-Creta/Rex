#pragma once

#include "rex_engine/event_system/event.h"
#include "rex_engine/input/mouse_position.h"

namespace rex
{
	class MouseMove : public Event<MouseMove>
	{
	public:
		MouseMove(MousePosition position)
			: m_position(position)
		{}

		MousePosition mouse_position() const
		{
			return m_position;
		}

	private:
		MousePosition m_position;
	};
}
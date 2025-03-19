#pragma once

#include "rex_engine/event_system/event.h"

#include "rex_engine/input/mouse_button.h"
#include "rex_engine/input/mouse_position.h"

namespace rex
{
	class MouseUp : public Event<MouseUp>
	{
	public:
		MouseUp(MouseButton mouseButton, MousePosition position)
			: m_mouse_button(mouseButton)
			, m_mouse_position(position)
		{}

		MouseButton mouse_button() const
		{
			return m_mouse_button;
		}
		MousePosition mouse_position() const
		{
			return m_mouse_position;
		}

	private:
		MouseButton m_mouse_button;
		MousePosition m_mouse_position;
	};
}
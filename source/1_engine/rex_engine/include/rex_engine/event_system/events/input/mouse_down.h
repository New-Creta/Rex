#pragma once

#include "rex_engine/input/mouse_button.h"
#include "rex_engine/input/mouse_position.h"
#include "rex_std/bonus/utility.h"

#include "rex_engine/event_system/event.h"

namespace rex
{
	DEFINE_YES_NO_ENUM(IsDoubleClick);

	class MouseDown : public Event<MouseDown>
	{
	public:
		MouseDown(MouseButton mouseButton, MousePosition position, IsDoubleClick isDoubleClick)
			: m_mouse_button(mouseButton)
			, m_position(position)
			, m_is_double_click(isDoubleClick)
		{}

		MouseButton mouse_button() const
		{
			return m_mouse_button;
		}
		MousePosition mouse_position() const
		{
			return m_position;
		}
		bool is_double_click() const
		{
			return m_is_double_click;
		}

	private:
		MouseButton m_mouse_button;
		MousePosition m_position;
		bool m_is_double_click;
	};
}
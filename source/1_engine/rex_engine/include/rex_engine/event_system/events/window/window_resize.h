#pragma once

#include "rex_engine/event_system/event.h"

namespace rex
{
	enum class WindowResizeType
	{
		Invalid,

		Restored,
		Minimized,
		Maximized,
		OtherRestored,
		OtherMaximized
	};

	class WindowResize : public Event<WindowResize>
	{
	public:
		WindowResize(card32 width, card32 height, WindowResizeType type)
			: m_width(width)
			, m_height(height)
			, m_type(type)
		{}

		card32 width() const
		{
			return m_width;
		}
		card32 height() const
		{
			return m_height;
		}
		WindowResizeType resize_type() const
		{
			return m_type;
		}

	private:
		card32 m_width;
		card32 m_height;
		WindowResizeType m_type;
	};
}
#pragma once

#include "rex_engine/event_system/event.h"
#include "rex_engine/engine/types.h"

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
		WindowResize(s32 width, s32 height, WindowResizeType type)
			: m_width(width)
			, m_height(height)
			, m_type(type)
		{}

		s32 width() const
		{
			return m_width;
		}
		s32 height() const
		{
			return m_height;
		}
		WindowResizeType resize_type() const
		{
			return m_type;
		}

	private:
		s32 m_width;
		s32 m_height;
		WindowResizeType m_type;
	};
}
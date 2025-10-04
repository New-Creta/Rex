#pragma once

#include "rex_engine/event_system/event.h"

namespace rex
{
	// An event that gets raised when a character is down
	// this is different than key down as a key is virtualized
	// an can be mapped to different characters
	// this event will hold the actual characters that's pressed
	class CharDown : public Event<CharDown>
	{
	public:
		CharDown(char8 key)
			: m_key(key)
		{}

		char8 key() const
		{
			return m_key;
		}

	private:
		char8 m_key;
	};
}
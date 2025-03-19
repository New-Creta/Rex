#pragma once

#include "rex_engine/event_system/event.h"

namespace rex
{
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
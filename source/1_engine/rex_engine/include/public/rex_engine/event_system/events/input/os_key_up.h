#pragma once

#include "rex_engine/event_system/event.h"
#include "rex_engine/input/key_code.h"

namespace rex
{
	// Key up event coming from the operating system
	class OSKeyUp : public Event<OSKeyUp>
	{
	public:
		OSKeyUp(KeyCode key)
			: m_key(key)
		{}

		KeyCode key() const
		{
			return m_key;
		}

	private:
		KeyCode m_key;
	};
}
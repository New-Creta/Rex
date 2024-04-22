#pragma once

#include "rex_engine/event_system/event.h"

#include "rex_engine/input/key_code.h"

namespace rex
{
	class KeyDown : public Event<KeyDown>
	{
	public:
		KeyDown(KeyCode key)
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
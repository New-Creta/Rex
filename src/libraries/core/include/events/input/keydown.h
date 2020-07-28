#pragma once

#include "rex_core_global.h"

#include "core/keycodes.h"

#include "events/input/keyevent.h"

namespace rex
{
	namespace events
	{
		class KeyDown : public KeyEvent
		{
		public:
			EVENT_CLASS_TYPE(EventType::KEY_PRESSED);

			REX_CORE_EXPORT KeyDown(rex::Window* window, const int8* key);
			REX_CORE_EXPORT ~KeyDown() override;

			REX_CORE_EXPORT KeyCode getKeyCode() const;

			REX_CORE_EXPORT std::string toString() const override;

		private:
			KeyCode m_key_code;
		};
	}
}
#pragma once

#include "rex_core_global.h"

#include "core/keycodes.h"

#include "events/input/keyevent.h"

namespace rex
{
	namespace events
	{
		class KeyUp : public KeyEvent
		{
		public:
			EVENT_CLASS_TYPE(EventType::KEY_RELEASED);

			REX_CORE_EXPORT KeyUp(rex::Window* window, const int8* key);
			REX_CORE_EXPORT ~KeyUp() override;

			REX_CORE_EXPORT std::string toString() const override;

        private:
            KeyCode m_key_code;
		};
	}
}
#pragma once

#include "rex_core_global.h"

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
		};
	}
}
#pragma once

#include "events/input/KeyUp.h"
#include "events/input/win32keystate.h"

#include "events/win32messageparameters.h"
#include "events/win32nativeevent.h"

namespace rex
{
	class Window;

	namespace win32
	{
		class KeyUp : public events::KeyUp, public NativeEvent
		{
		public:
			KeyUp(rex::Window* window, rex::win32::MessageParameters params);
			~KeyUp() final;

            //-------------------------------------------------------------------------
            inline const NativeEvent* getNativeEvent() const { return this; }

		private:
			KeyState m_key_state;
		};
	}
}
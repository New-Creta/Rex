#pragma once

#include "events/input/KeyDown.h"
#include "events/input/win32keystate.h"

#include "events/win32messageparameters.h"
#include "events/win32nativeevent.h"

namespace rex
{
	class Window;

	namespace win32
	{
		class KeyDown : public events::KeyDown, public NativeEvent
		{
		public:
			KeyDown(rex::Window* window, rex::win32::MessageParameters params);
			~KeyDown() final;

            //-------------------------------------------------------------------------
            inline const NativeEvent* getNativeEvent() const { return this; }

		private:
			KeyState m_key_state;
		};
	}
}
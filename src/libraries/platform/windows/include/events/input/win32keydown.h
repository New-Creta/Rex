#pragma once

#include "events/input/KeyDown.h"
#include "events/input/win32keystate.h"

#include "events/win32messageparameters.h"

namespace rex
{
	class Window;

	namespace win32
	{
		class KeyDown : public events::KeyDown
		{
		public:
			KeyDown(rex::Window* window, rex::win32::MessageParameters params);
			~KeyDown() final;

		private:
			KeyState m_key_state;
		};
	}
}
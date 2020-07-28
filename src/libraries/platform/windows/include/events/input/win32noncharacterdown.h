#pragma once

#include "events/input/keydown.h"
#include "events/input/win32keystate.h"

#include "events/win32messageparameters.h"

namespace rex
{
	class Window;

	namespace win32
	{
		class NonCharacterKeyDown : public events::KeyDown
		{
		public:
			NonCharacterKeyDown(rex::Window* window, rex::win32::MessageParameters params);
			~NonCharacterKeyDown() final;

		private:
			KeyState m_key_state;
		};
	}
}
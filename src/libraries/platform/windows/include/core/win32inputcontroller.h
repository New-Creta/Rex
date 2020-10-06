#pragma once

#include "math/point.h"

#include "core/keycodes.h"
#include "core/mousecodes.h"

namespace rex
{
	class Window;

	namespace win32
	{
		class InputController
		{
		public:
			InputController();
			~InputController();

			void update();

			rex::Point64 getMousePosition(bool previousFrame = false) const;
			rex::Point64 getMouseMovement() const;

			void showMouseCursor(bool show);

			bool IsMouseButtonDown(rex::MouseCode button) const;
			bool IsMouseButtonUp(rex::MouseCode button) const;
			bool IsMouseButtonPressed(rex::MouseCode button) const;
			bool IsMouseButtonReleased(rex::MouseCode button) const;

			bool isKeyDown(rex::KeyCode code) const;
			bool isKeyUp(rex::KeyCode code) const;
			bool isKeyPressed(rex::KeyCode code) const;
			bool isKeyReleased(rex::KeyCode code) const;
		private:
			unsigned char* m_current_keyboard_state;
			unsigned char* m_previous_keyboard_state;
			unsigned char* m_keyboard_state_0;
			unsigned char* m_keyboard_state_1;
			bool m_keyboard_state_0_active;

			rex::Point64 m_current_mouse_position;
			rex::Point64 m_old_mouse_position;
			rex::Point64 m_mouse_movement;

			Window* m_main_window;
		};
	}
}

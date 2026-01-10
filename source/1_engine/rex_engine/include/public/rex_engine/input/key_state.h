#pragma once

#include "rex_engine/input/input_action.h"
#include "rex_engine/engine/types.h"

namespace rex
{
	// Describes information about what input was given by the user
	struct KeyState
	{
		// the action that was given by the user
		InputAction action;

		// the number of ticks it was pressed. this is the number of completed ticks
		// meaning on the first tick it's pressed, this value is still 0
		// a negative value means how many frames ago it was released
		// this is useful information to have when you want to check
		// if something was released
		s32 ticks_pressed;

		bool is_pressed() const
		{
			return ticks_pressed == 0;
		}
		bool is_down() const
		{
			return ticks_pressed >= 1;
		}
		bool is_released() const
		{
			return ticks_pressed < 0;
		}
	};
}
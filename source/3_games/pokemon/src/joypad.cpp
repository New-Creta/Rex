#include "pokemon/joypad.h"
#include "pokemon/wram.h"
#include "pokemon/hram.h"
#include "pokemon/input_constants.h"
#include "pokemon/hardware_constants.h"

namespace pokemon
{
	void try_soft_reset()
	{
		delay_frame();
		// deselect (redundant)
		*rJOYP = 0x30;
		
		hram.hSoftReset--;

		if (hram.hSoftReset == 0)
		{
			soft_reset();
		}

		joypad();
	}

	void discard_button_presses()
	{

	}

	void joypad()
	{
		// Update the joypad state variables :
		// [hJoyReleased] keys released since last time
		// [hJoyPressed] keys pressed since last time
		// [hJoyHeld] currently pressed keys

		// hJoyReleased: (hJoyLast ^ hJoyInput)& hJoyLast
		// hJoyPressed:  (hJoyLast ^ hJoyInput)& hJoyInput
		constexpr s8 soft_reset_input = A_BUTTON + B_BUTTON + SELECT + START;
		if (hram.hJoyPressed == soft_reset_input)
		{
			try_soft_reset();
		}
	}
}
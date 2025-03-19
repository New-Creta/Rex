#pragma once

#include "rex_std/bonus/math/point.h"

namespace rex
{
	struct MousePosition
	{
		rsl::pointi32 local_pos;		// The position from the top left of the window
		rsl::pointi32 screen_pos;   // The position from the top left of the screen
	};
}
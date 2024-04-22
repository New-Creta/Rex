#pragma once

#include "rex_std/bonus/math/point.h"

namespace rex
{
	struct MousePosition
	{
		rsl::point local_pos;
		rsl::point screen_pos;
	};
}
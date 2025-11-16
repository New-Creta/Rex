#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
	// A text event is something like a billboard
	// Something static in the scene that when interacted with
	// displays some text on screen
	struct TextEvent // BgEvent in pokemon code
	{
		rsl::pointi8 pos;
		rsl::string text;
		s8 sign_id;
	};
}
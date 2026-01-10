#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/assets/map_objects/map_object.h"

namespace rex
{
	// A text event is something like a billboard
	// Something static in the scene that when interacted with
	// displays some text on screen
	// text events do not need a sprite as the sprite is part of the background
	struct TextEvent : public MapObject
	{

	};
}
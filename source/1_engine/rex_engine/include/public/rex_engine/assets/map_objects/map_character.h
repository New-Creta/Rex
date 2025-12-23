#pragma once

#include "rex_engine/assets/map_objects/map_object.h"

namespace rex
{
	enum class MapCharacterMovement
	{
		Walk, // The character is allowed to walk
		Stay  // The character will not walk
	};
	enum class MapCharacterMovementDirection
	{
		None,				// The character direction will not change from its initial state
		UpDown,			// The character will move up and down
		LeftRight,	// The character will move left and right
		AnyDir			// The character will move in any direction
	};

	struct MapCharacter : public MapObject
	{
		MapCharacterMovementDirection direction;
		MapCharacterMovement movement;

		rsl::pointi8 pos;

		// Future fields that will be present here
		// - animation
		// - text events
	};
}
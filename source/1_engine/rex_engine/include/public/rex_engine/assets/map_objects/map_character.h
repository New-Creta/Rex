#pragma once

#include "rex_engine/assets/map_objects/map_object.h"

namespace rex
{
	enum class MapCharacterMovement
	{
		Walk,
		Stay
	};
	enum class MapCharacterMovementDirection
	{
		None,
		UpDown,
		LeftRight,
		AnyDir
	};
	struct MapCharacter : public MapObject
	{
		MapCharacterMovementDirection direction;
		MapCharacterMovement movement;
	};
}
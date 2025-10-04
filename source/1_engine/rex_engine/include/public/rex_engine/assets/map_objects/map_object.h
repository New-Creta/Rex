#pragma once

#include "rex_std/string.h"
#include "rex_std/bonus/math.h"

#include "rex_engine/assets/asset.h"

namespace rex
{
	enum class MapObjectType
	{
		Character, // default
		Item,
		Trainer,
		Pokemon
	};

	// an object event is a dynamic object in the scene
	// that may change direction or position over time
	// the player can interact with them
	struct MapObject : public Asset // object event in pokemon code
	{
		rsl::string text_id;
		rsl::string sprite_id;

		rsl::pointi8 pos;
	};
}
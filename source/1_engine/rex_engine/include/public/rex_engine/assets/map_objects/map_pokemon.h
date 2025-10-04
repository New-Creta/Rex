#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/assets/map_objects/map_object.h"

namespace rex
{
	struct MapPokemon : public MapObject
	{
		s8 pokemon_id;
		s8 pokemon_level;
	};
}
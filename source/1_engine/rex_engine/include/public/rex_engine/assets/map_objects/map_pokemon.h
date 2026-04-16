#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/assets/map_objects/map_object.h"

namespace rex
{
	struct MapPokemon : public MapObject
	{
		s8 pokemon_id; // This will become some kind of GUID or pointer to the data they need
		s8 pokemon_level;

		rsl::string sprite_id;	// This will become some kind of GUID or pointer to the data they need
		rsl::pointi8 pos;
	};
}
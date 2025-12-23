#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/assets/map_objects/map_character.h"

namespace rex
{
	struct MapTrainer : public MapCharacter
	{
		s8 trainer_class; // This will become some kind of GUID or pointer to the data they need
		s8 trainer_number; // This will become some kind of GUID or pointer to the data they need
	};
}
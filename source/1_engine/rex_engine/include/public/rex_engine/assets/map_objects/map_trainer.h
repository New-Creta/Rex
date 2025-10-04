#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/assets/map_objects/map_character.h"

namespace rex
{
	struct MapTrainer : public MapCharacter
	{
		s8 trainer_class;
		s8 trainer_number;
	};
}
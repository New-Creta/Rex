#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/assets/map_objects/map_object.h"

namespace rex
{
	struct MapPickup : public MapObject
	{
		s8 item;
	};
}
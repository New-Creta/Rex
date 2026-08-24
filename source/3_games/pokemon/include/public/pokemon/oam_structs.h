#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/defines.h"

namespace pokemon
{
	enum class OAMAttributes
	{
		CGBPalleteMask	= 0b11,
		Bank						= BIT(3),
		DmgPalette			= BIT(4),
		FlipX						= BIT(5),
		FlipY						= BIT(6),
		BehindBg				= BIT(7)
	};

	struct OAMStruct
	{
		s8 y_coord;			// Y coordinate in pixels, starting from the top
		s8 x_coord;			// X coordinate in pixels, starting from the left
		s8 tile_id;			// Tile index to use from tilemap
		s8 attributes;	// Attributes of the object
	};
}
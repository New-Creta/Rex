#pragma once

#include "rex_std/bonus/math.h"
#include "rex_engine/engine/types.h"

namespace rex
{
	// A warp event is a square in the map
	// that'd teleport the player from one map to another
	// if dst_map_id == -1, that means the player should teleport
	// to the previous map
	struct WarpEvent
	{
		rsl::pointi8 pos;
		s8 dst_map_id;
		s8 dst_warp_id;
	};

}
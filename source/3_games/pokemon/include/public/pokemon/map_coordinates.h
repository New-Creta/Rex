#pragma once

#include "rex_std/bonus/math/point.h"

#include "rex_engine/engine/types.h"

namespace pokemon
{
	struct BlockCoord : rsl::pointi8 {};  // strong type
	struct SquareCoord : rsl::pointi8 {}; // strong type
	struct TileCoord : rsl::pointi8 {};   // strong type

	namespace coords
	{
		TileCoord player_pos_to_screen_top_left(TileCoord coord);

		TileCoord block_to_tile_coord(BlockCoord coord);
		BlockCoord tile_to_block_coord(TileCoord coord);
		TileCoord block_top_left_coord(TileCoord coord);

		s32 coord_to_index(rsl::pointi8 coord, s32 width);
		rsl::pointi8 index_to_coord(s32 idx, s32 width);

		rsl::pointi8 map_to_abs(rsl::pointi8 coord);
		rsl::pointi8 abs_to_map(rsl::pointi8 coord);

	}
}
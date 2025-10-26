#pragma once

#include "rex_engine/engine/types_2d.h"
#include "rex_engine/engine/defines.h"

namespace rex
{
	class Map;

	rsl::pointi32 size_in_px(const Map* map);
	rsl::point<TileCount> size_in_tiles(const Map* map);

	REX_STRONG_TYPE(BlockCoord, rsl::pointi8);
	REX_STRONG_TYPE(SquareCoord, rsl::pointi8);
	REX_STRONG_TYPE(TileCoord, rsl::pointi8);
	REX_STRONG_TYPE(PixelCoord, rsl::pointi16);

	struct WorldCoordConverter
	{
	public:
		BlockCoord to_block_coord(SquareCoord inCoord) const;
		BlockCoord to_block_coord(TileCoord inCoord) const;
		BlockCoord to_block_coord(PixelCoord inCoord) const;

		SquareCoord to_square_coord(BlockCoord inCoord) const;
		SquareCoord to_square_coord(TileCoord inCoord) const;
		SquareCoord to_square_coord(PixelCoord inCoord) const;

		TileCoord to_tile_coord(BlockCoord inCoord) const;
		TileCoord to_tile_coord(SquareCoord inCoord) const;
		TileCoord to_tile_coord(PixelCoord inCoord) const;

		PixelCoord to_pixel_coord(BlockCoord inCoord) const;
		PixelCoord to_pixel_coord(SquareCoord inCoord) const;
		PixelCoord to_pixel_coord(TileCoord inCoord) const;

	public:
		s32 num_pixels_per_tile;
		s32 num_tiles_per_square;
		s32 num_tiles_per_block;
	};

}
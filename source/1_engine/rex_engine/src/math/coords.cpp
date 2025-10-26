#include "rex_engine/math/coords.h"

#include "rex_engine/assets/blockset.h"

#include "rex_engine/assets/map.h"

namespace rex
{
	rsl::pointi32 size_in_px(const Map* map)
	{
		rsl::pointi8 tile_size = map->blockset()->tileset()->tile_size();
		rsl::pointi32 map_size{};

		map_size.x = tile_size.x * map->width().get();
		map_size.y = tile_size.y * map->height().get();

		return map_size;
	}
	rsl::point<TileCount> size_in_tiles(const Map* map)
	{
		rsl::point<TileCount> map_size{};

		map_size.x.get() = Block::num_tiles_per_column() * map->width().get();
		map_size.y.get() = Block::num_tiles_per_row() * map->height().get();

		return map_size;
	}

	BlockCoord WorldCoordConverter::to_block_coord(SquareCoord inCoord) const
	{
		TileCoord tile_coord = to_tile_coord(inCoord);
		BlockCoord coord{};
		coord.x = tile_coord.x / num_tiles_per_block;
		coord.y = tile_coord.y / num_tiles_per_block;

		return coord;
	}
	BlockCoord WorldCoordConverter::to_block_coord(TileCoord inCoord) const
	{
		BlockCoord coord{};
		coord.x = inCoord.x / num_tiles_per_block;
		coord.y = inCoord.y / num_tiles_per_block;

		return coord;
	}
	BlockCoord WorldCoordConverter::to_block_coord(PixelCoord inCoord) const
	{
		TileCoord tile_coord = to_tile_coord(inCoord);
		BlockCoord coord{};
		coord.x = tile_coord.x / num_tiles_per_block;
		coord.y = tile_coord.y / num_tiles_per_block;

		return coord;
	}

	SquareCoord WorldCoordConverter::to_square_coord(BlockCoord inCoord) const
	{
		TileCoord tile_coord = to_tile_coord(inCoord);
		SquareCoord coord{};
		coord.x = tile_coord.x / num_tiles_per_square;
		coord.y = tile_coord.y / num_tiles_per_square;

		return coord;
	}
	SquareCoord WorldCoordConverter::to_square_coord(TileCoord inCoord) const
	{
		SquareCoord coord{};
		coord.x = inCoord.x / num_tiles_per_square;
		coord.y = inCoord.y / num_tiles_per_square;

		return coord;
	}
	SquareCoord WorldCoordConverter::to_square_coord(PixelCoord inCoord) const
	{
		TileCoord tile_coord = to_tile_coord(inCoord);
		SquareCoord coord{};
		coord.x = tile_coord.x / num_tiles_per_square;
		coord.y = tile_coord.y / num_tiles_per_square;

		return coord;
	}

	TileCoord WorldCoordConverter::to_tile_coord(BlockCoord inCoord) const
	{
		TileCoord tile_coord{};
		tile_coord.x = inCoord.x * num_tiles_per_block;
		tile_coord.y = inCoord.y * num_tiles_per_block;
		return tile_coord;
	}
	TileCoord WorldCoordConverter::to_tile_coord(SquareCoord inCoord) const
	{
		TileCoord tile_coord{};
		tile_coord.x = inCoord.x * num_tiles_per_square;
		tile_coord.y = inCoord.y * num_tiles_per_square;
		return tile_coord;
	}
	TileCoord WorldCoordConverter::to_tile_coord(PixelCoord inCoord) const
	{
		TileCoord tile_coord{};
		tile_coord.x = inCoord.x / num_pixels_per_tile;
		tile_coord.y = inCoord.y / num_pixels_per_tile;
		return tile_coord;
	}

	PixelCoord WorldCoordConverter::to_pixel_coord(BlockCoord inCoord) const
	{
		TileCoord tile_coord = to_tile_coord(inCoord);
		PixelCoord coord{};
		coord.x = tile_coord.x * num_pixels_per_tile;
		coord.y = tile_coord.y * num_pixels_per_tile;
		return coord;
	}
	PixelCoord WorldCoordConverter::to_pixel_coord(SquareCoord inCoord) const
	{
		TileCoord tile_coord = to_tile_coord(inCoord);
		PixelCoord coord{};
		coord.x = tile_coord.x * num_pixels_per_tile;
		coord.y = tile_coord.y * num_pixels_per_tile;
		return coord;
	}
	PixelCoord WorldCoordConverter::to_pixel_coord(TileCoord inCoord) const
	{
		PixelCoord coord{};
		coord.x = inCoord.x * num_pixels_per_tile;
		coord.y = inCoord.y * num_pixels_per_tile;
		return coord;
	}

}
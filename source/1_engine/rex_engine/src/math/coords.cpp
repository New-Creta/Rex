#include "rex_engine/math/coords.h"

#include "rex_engine/assets/blockset.h"

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

}
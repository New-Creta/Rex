#include "pokemon/map_coordinates.h"

#include "pokemon/render_constants.h"

namespace pokemon
{
	namespace coords
	{
		TileCoord player_pos_to_screen_top_left(TileCoord coord)
		{
			// The player is always in tile coord {8, 8};

			TileCoord screen_top_left = coord;
			screen_top_left.x -= constants::player_render_position.x;
			screen_top_left.y -= constants::player_render_position.y;

			return screen_top_left;
		}

		TileCoord block_to_tile_coord(BlockCoord coord)
		{
			TileCoord tile_coord{};
			tile_coord.x = coord.x * constants::g_num_tiles_per_block_row;
			tile_coord.y = coord.y * constants::g_num_tiles_per_block_column;

			return tile_coord;
		}
		BlockCoord tile_to_block_coord(TileCoord coord)
		{
			BlockCoord block_coord{};
			block_coord.x = coord.x / constants::g_num_tiles_per_block_row;
			block_coord.y = coord.y / constants::g_num_tiles_per_block_column;

			return block_coord;
		}
		TileCoord block_top_left_coord(TileCoord coord)
		{
			BlockCoord block_coord = tile_to_block_coord(coord);
			return block_to_tile_coord(block_coord);
		}

		s32 coord_to_index(rsl::pointi8 coord, s32 width)
		{
			return coord.y * width + coord.x;
		}
		rsl::pointi8 index_to_coord(s32 idx, s32 width)
		{
			rsl::pointi8 coord{};

			coord.x = idx % width;
			coord.y = idx / width;

			return coord;
		}

		rsl::pointi8 map_to_abs(rsl::pointi8 coord)
		{
			rsl::pointi8 res{};

			res.x = coord.x + constants::g_map_padding_blocks;
			res.y = coord.y + constants::g_map_padding_blocks;

			return res;
		}
		rsl::pointi8 abs_to_map(rsl::pointi8 coord)
		{
			rsl::pointi8 res{};

			res.x = coord.x - constants::g_map_padding_blocks;
			res.y = coord.y - constants::g_map_padding_blocks;

			return res;
		}
	}
}
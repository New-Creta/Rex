#include "pokemon/game_blockmap.h"

#include "pokemon/render_constants.h"
#include "pokemon/map_coordinates.h"

#include "rex_engine/assets/blockset.h"

#include "rex_engine/shapes/rect.h"

#include "rex_engine/gfx/resources/texture_2d.h"

namespace pokemon
{
  // Returns the rex::Rect8 in which the connection map should be drawn
  rex::Rect8 rect_for_connection(const rex::Map* map, const rex::MapConnection & conn)
  {
    // Maps are always center aligned, their midpoint shifts depending on the connection offset
    s8 mid_point_x = constants::g_map_padding_blocks + ((map->desc().map_header.width_in_blocks / 2) + conn.offset);
    s8 mid_point_y = constants::g_map_padding_blocks + ((map->desc().map_header.height_in_blocks / 2) + conn.offset);

    s8 start = 0;
    s8 end = 0;
    // The formula to calculate the start is the same for each direction, just takes different input
    // depending on the connection's direction
    if (conn.direction == rex::Direction::North || conn.direction == rex::Direction::South)
    {
      start = static_cast<s8>(rsl::clamp_min(mid_point_x - (conn.map->desc().map_header.width_in_blocks / 2) + conn.offset, 0));
      s8 depth = static_cast<s8>(rsl::min(map->desc().map_header.width_in_blocks + constants::g_map_padding_blocks, static_cast<s32>(conn.map->desc().map_header.width_in_blocks)));
      end = static_cast<s8>(start + depth);
    }
    else
    {
      start = static_cast<s8>(rsl::clamp_min(mid_point_y - (conn.map->desc().map_header.height_in_blocks / 2) + conn.offset, 0));
      s8 depth = static_cast<s8>(rsl::min(map->desc().map_header.height_in_blocks + constants::g_map_padding_blocks, static_cast<s32>(conn.map->desc().map_header.height_in_blocks)));
      end = static_cast<s8>(start + depth);
    }

    // Fill in the rex::Rect8 based on the direction of the connection
    rex::Rect8 res{};

    switch (conn.direction)
    {
    case rex::Direction::North:
      res.top_left.x = start;
      res.top_left.y = 0;
      res.bottom_right.x = end;
      res.bottom_right.y = res.top_left.y + constants::g_map_padding_blocks;
      break;
    case rex::Direction::East:
      res.top_left.x = map->desc().map_header.width_in_blocks + constants::g_map_padding_blocks;
      res.top_left.y = start;
      res.bottom_right.x = res.top_left.x + constants::g_map_padding_blocks;
      res.bottom_right.y = end;
      break;
    case rex::Direction::South:
      res.top_left.x = start;
      res.top_left.y = map->desc().map_header.height_in_blocks + constants::g_map_padding_blocks;
      res.bottom_right.x = end;
      res.bottom_right.y = res.top_left.y + constants::g_map_padding_blocks;
      break;
    case rex::Direction::West:
      res.top_left.x = 0;
      res.top_left.y = start;
      res.bottom_right.x = res.top_left.x + constants::g_map_padding_blocks;
      res.bottom_right.y = end;
      break;
    }

    return res;
  }

  rsl::pointi8 project_point_to_conn(const rex::Map* mapObject, const rex::MapConnection& conn, rsl::pointi8 coord)
  {
    s8 projected_point = 0;
    if (conn.direction == rex::Direction::North || conn.direction == rex::Direction::South)
    {
      s8 mid_point_x = conn.map->desc().map_header.width_in_blocks / 2 - conn.offset;
      s8 offset_from_mid = constants::g_map_padding_blocks + (mapObject->desc().map_header.width_in_blocks / 2) - coord.x;
      projected_point = mid_point_x - offset_from_mid;
    }
    else
    {
      s8 mid_point_y = conn.map->desc().map_header.height_in_blocks / 2 - conn.offset;
      s8 offset_from_mid = constants::g_map_padding_blocks + (mapObject->desc().map_header.height_in_blocks / 2) - coord.y;
      projected_point = mid_point_y - offset_from_mid;
    }

    rsl::pointi8 res{};

    switch (conn.direction)
    {
    case rex::Direction::North:
      res.x = projected_point;
      res.y = conn.map->desc().map_header.height_in_blocks - constants::g_map_padding_blocks;
      break;
    case rex::Direction::East:
      res.x = 0;
      res.y = projected_point;
      break;
    case rex::Direction::South:
      res.x = projected_point;
      res.y = 0;
      break;
    case rex::Direction::West:
      res.x = conn.map->desc().map_header.width_in_blocks - constants::g_map_padding_blocks;
      res.y = projected_point;
      break;
    }

    return res;
  }


	struct GameTilemapParams
	{
		s32 width;
		s32 height;
		rex::Block* border_block;
		rex::Map* map;
		rex::Map* connections[4];
	};

	GameBlockMap::GameBlockMap(const rex::Map* map)
		: rex::Tilemap(
			map->width_in_tiles() / constants::g_num_tiles_per_block_row + (2 * constants::g_map_padding_blocks),
			map->height_in_tiles() / constants::g_num_tiles_per_block_column + (2 * constants::g_map_padding_blocks))
	{
    m_blocks = rsl::make_unique<u8[]>(width_in_blocks() * height_in_blocks());
		
		init_border_blocks(m_blocks.get(), m_blocks.count(), map);
		init_connection_blocks(m_blocks.get(), m_blocks.count(), map);
		init_inner_map_blocks(m_blocks.get(), m_blocks.count(), map);

		convert_blocks_to_tiles(m_blocks.get(), m_blocks.count(), map);
	}

	void GameBlockMap::init_border_blocks(u8* blocks, s32 numBlocks, const rex::Map* map)
	{
		rsl::fill_n(blocks, numBlocks, map->desc().map_header.border_block_idx);
	}
	void GameBlockMap::init_connection_blocks(u8* blocks, s32 numBlocks, const rex::Map* map)
	{
		for (const rex::MapConnection & conn : map->desc().connections)
		{
			// Calculate the rect in which the connection will be drawn
			// This rect covers the entire padding region
			rex::Rect8 rect = rect_for_connection(map, conn);

			// Calculate the top left position where the drawing will start, inside of this rect
			rsl::pointi8 top_left_conn = project_point_to_conn(map, conn, rect.top_left);

			// Load the map blocks of the connection so we can assign the right block index to the map matrix
      const u8* conn_map_blocks = conn.map->blocks();

			// Go over the blocks of the connection and assign the block index to the map matrix
			for (s8 y = rect.top_left.y, conn_y = top_left_conn.y; y < rect.bottom_right.y; ++y, ++conn_y)
			{
				for (s8 x = rect.top_left.x, conn_x = top_left_conn.x; x < rect.bottom_right.x; ++x, ++conn_x)
				{
					// Look up the block index of the current block we're looking at in the connection map
					s16 conn_idx = conn_y * conn.map->desc().map_header.width_in_blocks + conn_x;
					u8 block_idx = conn_map_blocks[conn_idx];

					// Assign the connection's block index to the map matrix
					s16 index = (y * width_in_blocks()) + x;
					blocks[index] = block_idx;
				}
			}
		}
	}
	void GameBlockMap::init_inner_map_blocks(u8* blocks, s32 numBlocks, const rex::Map* map)
	{
    s32 height = map->desc().map_header.height_in_blocks;
    s32 width = map->desc().map_header.width_in_blocks;

    const u8* map_blocks = map->blocks();
    for (s8 y = 0; y < height; ++y)
    {
      for (s8 x = 0; x < width; ++x)
      {
        s32 index = y * width + x;
        s16 map_matrix_index = ((y + constants::g_map_padding_blocks) * width_in_blocks()) + (x + constants::g_map_padding_blocks);
        blocks[map_matrix_index] = map_blocks[index++];
      }
    }
	}

  void GameBlockMap::convert_blocks_to_tiles(u8* blocks, s32 numBlocks, const rex::Map* map)
  {
    s32 tiles_per_row = map->blockset()->tileset()->tileset_texture()->texture_resource()->width() / constants::g_tile_width_px;

    // Start the loop from this block, going left to right, top to down
    // Restricting to only the tiles that'll be rendered
    rsl::unique_array<u8> tiles = rsl::make_unique<u8[]>(width_in_tiles()* height_in_tiles());

    s32 current_tile_in_cache_idx = 0;
    for (s32 y = 0; y < height_in_tiles(); ++y)
    {
      for (s32 x = 0; x < width_in_tiles(); ++x)
      {
        // Get the tile coord of the tile we're currently processing
        TileCoord coord{};
        TileCoord block_coord{};
        coord.x += static_cast<s8>(x);
        coord.y += static_cast<s8>(y);
        block_coord.x = coord.x / 4;
        block_coord.y = coord.y / 4;

        // Get the block the tile belongs to
        s32 block_idx = coords::coord_to_index(block_coord, width_in_blocks());
        s32 block_idx_idx = m_blocks[block_idx];
        const rex::Block& block = map->blockset()->block(block_idx_idx);

        // Get the tile coordinate of the the first tile in the block (which is top left)
        TileCoord block_top_left = coords::block_top_left_coord(coord);

        // Get the relative vector from this first tile to the current tile
        // Based on that, calculate which tile in the block we're currently processing
        rsl::pointi8 coord_rel_to_block = coord - block_top_left;
        u8 tile_idx = block.index_at(coord_rel_to_block);

        // Store the tile index in the cache
        tiles[current_tile_in_cache_idx] = tile_idx;
        rsl::pointi8 tile_coord = coords::index_to_coord(tile_idx, tiles_per_row);
        current_tile_in_cache_idx++;
      }
    }

    init_tiles(rsl::move(tiles));
  }
}
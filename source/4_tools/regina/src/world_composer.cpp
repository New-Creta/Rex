#include "regina/world_composer.h"

#include "rex_engine/engine/asset_db.h"

#include "rex_std/vector.h"

namespace regina
{
	DEFINE_LOG_CATEGORY(LogWorldComposer);

	namespace
	{
		rsl::pointi32 block_to_tile_coord(rsl::pointi32 coord)
		{
			rsl::pointi32 tile_coord{};
			tile_coord.x = coord.x * 4;
			tile_coord.y = coord.y * 4;

			return tile_coord;
		}
		rsl::pointi32 tile_to_block_coord(rsl::pointi32 coord)
		{
			rsl::pointi32 block_coord{};
			block_coord.x = coord.x / 4;
			block_coord.y = coord.y / 4;

			return block_coord;
		}
		rsl::pointi32 block_top_left_coord(rsl::pointi32 coord)
		{
			rsl::pointi32 block_coord = tile_to_block_coord(coord);
			return block_to_tile_coord(block_coord);
		}
	}

	bool WorldComposer::is_map_in_tilemap(const rex::Map* map) const
	{
		if (!map)
		{
			return false;
		}

		return m_map_to_metadata.contains(map);
	}
	rsl::pointi32 WorldComposer::map_pos(const rex::Map* map) const
	{
		const rex::MinMax& aabb = m_map_to_metadata.at(map).aabb;

		rsl::pointi32 pos = aabb.min;
		pos.x += aabb.width() / 2;
		pos.y += aabb.height() / 2;

		return pos;
	}
	const rex::Tilemap* WorldComposer::tilemap() const
	{
		return m_tilemap.get();
	}

	void WorldComposer::build_world(rex::Map* startMap)
	{
		// Loop over all the maps we have, starting from the first and save their relative position
		// Afterwards, creating a AABB over that encapsulates all the AABBs of every map and change the origin to point to the top left of the encapsulating AABB
		//
		//                                              --- EXAMPLE ---
		// In the example below, we start from map A and recursively loop over all connections until we've explored all maps
		// this means that all map AABB would be relative from map A
		// we need to calculate the AABB that encapsulates all maps and then recalculate the AABB of each map relative to the origin of the encapsulating AABB
		//
		// +-----------------------------------------------------------------------------------------------------------------------+
		// |																																																											 |
		// |																																					+--------------------------+								 |
		// |		+--------------------------+																					|													 |								 |
		// |		|                          |																					|													 |								 |
		// |		|                          |																					|													 |								 |
		// |		|             A            |  ----------------------------------------|					  	C						 |								 |
		// |		|                          |																					|													 |								 |
		// |		|                          |																					|													 |								 |
		// |		|                          |																					|													 |								 |
		// |		+--------------------------+																					+--------------------------+								 |
		// |									|                                                                     |															 |
		// |									|																																			|															 |
		// |									|																																			|															 |
		// |									|																																			|															 |
		// |									|																																			|															 |
		// |									|																																			|															 |
		// |									|																																			|															 |
		// |									|																																			|															 |
		// |									|																																			|															 |
		// |									|																																			|															 |
		// |								  |																																			|									  					 |
		// |		+--------------------------+																					+--------------------------+								 |
		// |		|                          | 																					|                          |								 |
		// |		|                          | 																					|                          |								 |
		// |		|                          | 																					|                          |								 |
		// |		|             B            | ---------------------------------------- |             D            |	  						 |
		// |		|                          | 																					|                          |								 |
		// |		|                          | 																					|                          |								 |
		// |		|                          | 																					|                          |								 |
		// |		+--------------------------+																					+--------------------------+								 |
		// |																																																											 |
		// |																																																											 |
		// +-----------------------------------------------------------------------------------------------------------------------+
		// 
		// 
		// 
		// 
		//



		// Clear previous loaded entries
		m_map_to_metadata.clear();

		load_all_maps(startMap);
		build_world_in_relative_coord(startMap);
		convert_world_to_abs_coord();
	}

	void WorldComposer::load_all_maps(rex::Map* startMap)
	{
		// Load all maps so we can run through them very quickly later on

		rsl::vector<rex::Map*> open_nodes = { startMap };
		rsl::vector<const rex::Map*> closed_nodes;

		while (!open_nodes.empty())
		{
			rex::Map* current_node = open_nodes.back();
			open_nodes.pop_back();
			if (rsl::find(closed_nodes.cbegin(), closed_nodes.cend(), current_node) != closed_nodes.cend())
			{
				continue;
			}

			closed_nodes.push_back(current_node);
			rex::asset_db::instance()->hydrate_asset(current_node);

			for (const rex::MapConnection& conn : current_node->desc().connections)
			{
				open_nodes.push_back(conn.map);
			}
		}
	}
	void WorldComposer::build_world_in_relative_coord(const rex::Map* startMap)
	{
		struct MapWithPos
		{
			const rex::Map* map;
			rsl::pointi32 pos;
		};

		std::vector<MapWithPos> open_nodes;
		std::vector<MapWithPos> closed_nodes;

		open_nodes.push_back({ startMap, rsl::pointi32{} });

		while (!open_nodes.empty())
		{
			MapWithPos current_node = open_nodes.back();
			open_nodes.pop_back();

			// skip any maps that are already processed
			auto closed_node_it = std::find_if(closed_nodes.cbegin(), closed_nodes.cend(), [&](const MapWithPos& mapWithPos)
				{
					return mapWithPos.map == current_node.map;
				});
			if (closed_node_it != closed_nodes.cend())
			{
				continue;
			}

			// 1. calculate bounding box of current map and cache it
			rex::MinMax map_rect = calc_map_aabb(current_node.map->desc().map_header, current_node.pos);
			MapMetaData map_meta_data{ map_rect };
			m_map_to_metadata.emplace(current_node.map, map_meta_data);

			// 2. add the map itself to the closed nodes
			closed_nodes.push_back(current_node);

			// 3. add the map's connections to the open nodes
			for (const rex::MapConnection& conn : current_node.map->desc().connections)
			{
				// skip any maps that are already processed
				closed_node_it = std::find_if(closed_nodes.cbegin(), closed_nodes.cend(), [&](const MapWithPos& mapWithPos)
					{
						return mapWithPos.map == conn.map;
					});
				if (closed_node_it != closed_nodes.cend())
				{
					continue;
				}

				REX_STATIC_WARNING("Load tiles per block from tileset/config");
				const s32 tiles_per_block = 4;
				s32 half_width_in_tiles = (current_node.map->desc().map_header.width_in_blocks * tiles_per_block / 2);
				s32 half_height_in_tiles = (current_node.map->desc().map_header.height_in_blocks * tiles_per_block / 2);
				s32 half_conn_width_in_tiles = (conn.map->desc().map_header.width_in_blocks * tiles_per_block / 2);
				s32 half_conn_height_in_tiles = (conn.map->desc().map_header.height_in_blocks * tiles_per_block / 2);
				rsl::pointi32 conn_pos;

				// calculate the middle position of the connecting map
				// a connection always tries to connect the middle of the 2 maps together
				// We use the offset, which is part of connection data and is represented in tiles, to offset in a perpendicluar direction from the connection
				// ie left or right for a north or south direction or up or down for a east or west direction
				// the resulting connection pos calculated here is the middle point of the connecting map
				if (conn.direction == rex::Direction::North)
				{
					conn_pos = { current_node.pos.x + conn.offset, current_node.pos.y - half_height_in_tiles - half_conn_height_in_tiles };
				}
				else if (conn.direction == rex::Direction::South)
				{
					conn_pos = { current_node.pos.x + conn.offset, current_node.pos.y + half_height_in_tiles + half_conn_height_in_tiles };
				}
				else if (conn.direction == rex::Direction::East)
				{
					conn_pos = { current_node.pos.x + half_width_in_tiles + half_conn_width_in_tiles, current_node.pos.y + conn.offset };
				}
				else if (conn.direction == rex::Direction::West)
				{
					conn_pos = { current_node.pos.x - half_width_in_tiles - half_conn_width_in_tiles, current_node.pos.y + conn.offset };
				}

				open_nodes.push_back({ conn.map, conn_pos });
			}
		}
	}
	void WorldComposer::convert_world_to_abs_coord()
	{
		// The min max results are stored relative from the active map
		// We need to convert them to absolute positions
		// We do this by getting the lowest possible point in the relative position
		// and converting that point to be our origin
		rex::MinMax big_aabb;
		for (const auto& [map, metadata] : m_map_to_metadata)
		{
			big_aabb.min.x = rsl::min(big_aabb.min.x, metadata.aabb.min.x);
			big_aabb.min.y = rsl::min(big_aabb.min.y, metadata.aabb.min.y);
			big_aabb.max.x = rsl::max(big_aabb.max.x, metadata.aabb.max.x);
			big_aabb.max.y = rsl::max(big_aabb.max.y, metadata.aabb.max.y);
		}
		s32 width = big_aabb.width();
		s32 height = big_aabb.height();

		// Now go over all the minmax results and convert their coordinates
		for (auto& [map, metadata] : m_map_to_metadata)
		{
			metadata.aabb.min.x -= big_aabb.min.x;
			metadata.aabb.min.y -= big_aabb.min.y;
			metadata.aabb.max.x -= big_aabb.min.x;
			metadata.aabb.max.y -= big_aabb.min.y;
		}

		// Create the tilemap and fill in the tile values of each map
		s32 tiles_per_block = 1;
		m_tilemap = rsl::make_unique<rex::Tilemap>(rsl::point<rex::TileCount>{ rex::TileCount(width * tiles_per_block), rex::TileCount(height * tiles_per_block) });

		for (const auto& [map, metadata] : m_map_to_metadata)
		{
			s32 map_width = metadata.aabb.width() / 4;
			s32 map_height = metadata.aabb.height() / 4;

			rsl::pointi32 top_left = metadata.aabb.min; // top left position of map, in block coordinate

			tiles_per_block = 4;
			s32 map_width_in_tiles = map_width * tiles_per_block;
			s32 map_height_in_tiles = map_height * tiles_per_block;

			s32 top_left_tile_start_idx = top_left.y * width + top_left.x;
			for (s32 y = 0; y < map_height_in_tiles; ++y)
			{
				s32 current_tile_idx = top_left_tile_start_idx + y * width;
				for (s32 x = 0; x < map_width_in_tiles; ++x)
				{
					// Get the tile coord of the tile we're currently processing
					rsl::pointi32 coord{};
					rsl::pointi32 block_coord{};
					coord.x += static_cast<s8>(x);
					coord.y += static_cast<s8>(y);
					block_coord.x = coord.x / 4;
					block_coord.y = coord.y / 4;

					// Get the block the tile belongs to
					s32 block_idx = block_coord.y * map_width + block_coord.x;
					s32 block_idx_idx = *map->blocks(block_idx);
					const rex::Block& block = map->blockset()->block(block_idx_idx);

					// Get the tile coordinate of the the first tile in the block (which is top left)
					rsl::pointi32 block_top_left = block_top_left_coord(coord);

					// Get the relative vector from this first tile to the current tile
					// Based on that, calculate which tile in the block we're currently processing
					rsl::pointi32 coord_rel_to_block = coord - block_top_left;
					u8 tile_idx = block.index_at(rsl::pointi8{ (s8)coord_rel_to_block.x, (s8)coord_rel_to_block.y });

					// Store the tile index in the cache
					m_tilemap->set(current_tile_idx + x, tile_idx);
				}
			}
		}
	}
	rex::MinMax WorldComposer::calc_map_aabb(const rex::MapHeader& mapHeader, rsl::pointi32 startPos)
	{
		s32 half_width_in_tiles = (mapHeader.width_in_blocks * 4/ 2);
		s32 half_height_in_tiles = (mapHeader.height_in_blocks * 4 / 2);

		rex::MinMax res{};
		res.min.x = -half_width_in_tiles + startPos.x;
		res.min.y = -half_height_in_tiles + startPos.y;
		res.max.x = half_width_in_tiles + startPos.x;
		res.max.y = half_height_in_tiles + startPos.y;

		return res;
	}
}
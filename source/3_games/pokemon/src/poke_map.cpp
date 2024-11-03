#include "pokemon/poke_map.h"

#include "rex_engine/gfx/primitives/box.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/memory/blob.h"
#include "rex_engine/memory/blob_reader.h"

#include "rex_engine/gfx/system/rhi.h"
#include "rex_engine/gfx/resources/texture_2d.h"

#include "glm/glm.hpp"
#include "rex_engine/images/stb_image.h"

#include "rex_engine/components/transform_component.h"
#include "rex_std/bonus/cinttypes.h"
#include "rex_std/limits.h"
#include "rex_engine/text_processing/text_processing.h"
#include "rex_std/bonus/math/point.h"

// Game Boy & Game Boy Color Resolution
// 160 x 144
// Tileset: 8x8
// 
// Player is always located at x=10,y=9
// with top left as origin
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   | x |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
// |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+


namespace pokemon
{
	struct MapHeader;

	DEFINE_LOG_CATEGORY(LogMap);

	struct connection
	{
		s8 map_id;
		void* blocks;
		MapHeader* map;
		s8 size;
		s8 width;
		s8 y;
		s8 x;
		s16* window;
	};

	struct MapHeader
	{
		s8 tileset_id;
		s8 height;
		s8 width;
		void* blocks;
		void* text;
		void* script;
		s8 connections_bitfield; // bit fields indicating which connections we have
		rsl::vector<connection> connections;
		void* object;
	};

	s32 block_num_to_width_pixels(s32 numBlocks)
	{
		return numBlocks * Block::s_width_px;
	}
	s32 block_num_to_height_pixels(s32 numBlocks)
	{
		return numBlocks * Block::s_height_px;
	}

	// Layout of a map texture, each cell represents a tile
	// 
	// 1 == padding tile
	// 2 == connection tile
	// 3 == map tile
	// 
	//
	//  padding tiles                   possible connection tiles                   padding tiles
	//       |																    |																	     |
	//   +---+---+														+---+---+															 +---+---+
	//   |       |														|       |															 |       |
	//   v       v														v       v															 v       v
	//                               
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	// |###|###|###|###|###|###|###|###|###| c | c | c |###|###|###|###|###|###|###|###|###|   <--+
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+      |
	// |###|###|###|###|###|###|###|###|###| c | c | c |###|###|###|###|###|###|###|###|###|      +---- padding tiles
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+      |
	// |###|###|###|###|###|###|###|###|###| c | c | c |###|###|###|###|###|###|###|###|###|   <--+
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	// |###|###|###| m | m | m | m | m | m | m | m | m | m | m | m | m | m | m |###|###|###|
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	// |###|###|###| m | m | m | m | m | m | m | m | m | m | m | m | m | m | m |###|###|###|
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	// |###|###|###| m | m | m | m | m | m | m | m | m | m | m | m | m | m | m |###|###|###|
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	// |###|###|###| m | m | m | m | m | m | m | m | m | m | m | m | m | m | m |###|###|###|
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	// |###|###|###| m | m | m | m | m | m | m | m | m | m | m | m | m | m | m |###|###|###|
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	// |###|###|###| m | m | m | m | m | m | m | m | m | m | m | m | m | m | m |###|###|###|
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	// |###|###|###| m | m | m | m | m | m | m | m | m | m | m | m | m | m | m |###|###|###|
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	// |###|###|###| m | m | m | m | m | m | m | m | m | m | m | m | m | m | m |###|###|###|
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	// |###|###|###| m | m | m | m | m | m | m | m | m | m | m | m | m | m | m |###|###|###|
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	// |###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|  <--+
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+	   |
	// |###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|	   +---- padding tiles
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+	   |
	// |###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|###|	<--+
	// +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+

	struct MapHeader;
	struct MapConnection
	{
		MapHeader* map;
		Direction direction;
		s8 offset;
	};

	struct WrapEvent
	{
		s8 x;
		s8 y;
		s8 dst_map_id;
		s8 dst_warp_id;
	};

	struct TextEvent // BgEvent in pokemon code
	{
		s8 x;
		s8 y;
		s8 sign_id;
	};

	struct ObjectEvent
	{
		s8 x;
		s8 y;
		s8 sprite_id;
		s8 movement; // walk/stay
		s8 range_or_direction;
		s8 text_id;
	};
	struct ItemObjectEvent : ObjectEvent
	{
		s8 item;
	};
	struct TrainerObjectEvent : ObjectEvent
	{
		s8 trainer_class;
		s8 trainer_number;
	};
	struct PokemonObjectEvent : ObjectEvent
	{
		s8 pokemon_id;
		s8 pokemon_level;
	};

	struct Trainer
	{
		s8 event_flag;
		s8 view_range;
		s8 text_before_battle;
		s8 text_after_battle;
		s8 text_end_battle;
	};

	struct MapHeader
	{
		rsl::string name;
		rsl::string map_blocks;
		rsl::string blockset;
		rsl::vector<MapConnection> connections;
		rsl::vector<WrapEvent> wrap_events;
		s8 width;		// represented in block count
		s8 height;	// represented in block count
		s8 border_block_idx; // Index of the block used for the border if no connection blocks are present

	};

	class MapTexture
	{
	private:
		s8 total_width() const
		{
			return m_width + (2 * s_num_padding_blocks);
		}
		s8 total_height() const
		{
			return m_height + (2 * s_num_padding_blocks);
		}

		void assign_block(s8 x, s8 y, s8 id)
		{
			s32 idx = y * m_width + x;
			m_block_ids[idx] = id;
		}

		void fill_border_block(s8 borderBlockId)
		{
			// Top
			for (s32 y = 0; y < s_num_padding_blocks; ++y)
			{
				for (s32 x = 0; x < m_width + s_num_padding_blocks; ++x)
				{
					assign_block(x, y, borderBlockId);
				}
			}

			// Left
			for (s32 y = s_num_padding_blocks; y < m_height; ++y)
			{
				for (s32 x = 0; x < s_num_padding_blocks; ++x)
				{
					assign_block(x, y, borderBlockId);
				}
			}

			// Right
			for (s32 y = s_num_padding_blocks; y < m_height; ++y)
			{
				for (s32 x = m_width; x < m_width + s_num_padding_blocks; ++x)
				{
					assign_block(x, y, borderBlockId);
				}
			}

			// Bottom
			for (s32 y = m_height; y < m_height + s_num_padding_blocks; ++y)
			{
				for (s32 x = 0; x < m_width + s_num_padding_blocks; ++x)
				{
					assign_block(x, y, borderBlockId);
				}
			}
		}

		void fill_inner_map(rsl::string_view mapBlocks, rsl::string_view blockset)
		{
			rsl::unique_array<s8> map_blocks = load_map_blocks(mapBlocks);
			BlockSet* blockset = load_blockset(blockset);
			for (s8 y = 0; y < m_height; ++y)
			{
				for (s8 x = 0; x < m_width; ++x)
				{
					s16 map_blocks_idx = y * m_width + x;
					s8 block_idx = map_blocks[map_blocks_idx];
					assign_block(x, y, block_idx);
				}
			}
		}

		void fill_north_connection(const MapConnection& connection)
		{
			REX_ASSERT_X(connection.direction == Direction::North, "invalid direction for north connection");

			// A northern connection will always have its first block drawn at Y position 0
			// However, its X coordinate is not as easy to determine as it depends on the map we're connecting to
			// If the connecting map is of the same width as the current map and we're fully aligned (no offset)
			// Then top left X coordinate is 0
			// However, this is often not the case and we need to calculate the X coordinate based on the current map width
			// the connecting map's width and the offset of the connection
			// we should always try to draw an entire row equal to the width of the currnent map + the border block padding
			// However is the connecting map is too small or we can't draw an entire row due to the offset, we leave blanks
			// If the connecting map is bigger however, we should only draw those blocks that we can see

			s32 target_width_to_draw = total_width();
			s32 connecting_map_width = connection.map->width;

			// calculate which blocks of the connecting map we can draw
			// We start from the center X coordinate of the connecting map and move all the way to the right.
			// At the end we add the offset as it's possible the map got moved
			s32 start_x = (connecting_map_width / 2) - (m_width / 2) - s_num_padding_blocks + connection.offset;

			// it's possible the above start x coordinate is below 0, which means we need to shift where we start drawing in the map texture
			s32 x = 0;
			if (start_x < 0)
			{
				x -= start_x;
			}

			rsl::pointi8 connection_point_on_other_map{};
			connection_point_on_other_map.x = (connection.map->width / 2) - connection.offset;
			s8 num_tiles_needed = (m_width / 2 + s_num_padding_blocks);
			s8 num_tiles_left = rsl::clamp_max(num_tiles_needed, connection_point_on_other_map.x);

			// max width == entire span of map
			rsl::pointi8 top_left = top_mid;
			s8 half_conn_map_width = (connection.map->width) / 2;
			top_left.x = rsl::clamp_min(top_mid.x - half_conn_map_width, 0);
		}
		void fill_east_connection(const MapConnection& connection)
		{
			REX_ASSERT_X(connection.direction == Direction::East, "invalid direction for east connection");
		}
		void fill_south_connection(const MapConnection& connection)
		{
			REX_ASSERT_X(connection.direction == Direction::South, "invalid direction for south connection");
		}
		void fill_west_connection(const MapConnection& connection)
		{
			REX_ASSERT_X(connection.direction == Direction::West, "invalid direction for west connection");
		}

		void fill_connections(const rsl::vector<MapConnection>& connections)
		{
			if (connections.empty())
			{
				return;
			}

			for (const MapConnection& connection : connections)
			{
				// A connection is always positions at the middle of the current map and we use the offset
				// to move it left or right (for north/south connections) or up or down (for east/west connections)
				// This connection value is in square (2x2 tiles) 

				// Calculate the band of tiles of the connection we have to draw
				struct ConnectionTileBand
				{
					s32 x_start;
					s32 x_end;
					s32 y_start;
					s32 y_end;
				};

				s32 x = 0;
				s32 y = 0;
				s32 conn_map_x = 0;
				s32 conn_map_y = 0;
				ConnectionTileBand ctb{};
				switch (connection->direction)
				{
				case pokemon::Direction::North:
					ctb.y_start = connection->map->height - s_num_padding_blocks;
					ctb.y_end = connection->map->height;
					s32 mid_point = ((num_blocks_width / 2) + connection->offset);
					ctb.x_start = mid_point - (num_blocks_width / 2) - s_num_padding_blocks;
					ctb.x_end = mid_point + (num_blocks_width / 2) + s_num_padding_blocks;
					ctb.x_start = rsl::clamp_min(ctb.x_start, 0);
					ctb.x_end = rsl::clamp_max(ctb.x_end, connection->map->width);

					y = 0;
					x = mid_point - ctb.x_start;
				case pokemon::Direction::East:
					ctb.x_start = 0;
					ctb.x_end = s_num_padding_blocks;
					s32 mid_point = ((num_blocks_height / 2) + connection->offset);
					ctb.y_start = mid_point - (num_blocks_height / 2) - s_num_padding_blocks;
					ctb.y_end = mid_point + (num_blocks_height / 2) + s_num_padding_blocks;
					ctb.y_start = rsl::clamp_min(ctb.y_start, 0);
					ctb.y_end = rsl::clamp_max(ctb.y_end, connection->map->height);

					x = total_blocks_width - s_num_padding_blocks;
					y = mid_point - ctb.y_start;
				case pokemon::Direction::South:
					ctb.y_start = 0;
					ctb.y_end = s_num_padding_blocks;
					s32 mid_point = ((num_blocks_width / 2) + connection->offset);
					ctb.x_start = mid_point - (num_blocks_width / 2) - s_num_padding_blocks;
					ctb.x_end = mid_point + (num_blocks_width / 2) + s_num_padding_blocks;
					ctb.x_start = rsl::clamp_min(ctb.x_start, 0);
					ctb.x_end = rsl::clamp_max(ctb.x_end, connection->map->width);

					y = total_blocks_height - s_num_padding_blocks;
					x = mid_point - ctb.x_start;
				case pokemon::Direction::West:
					ctb.x_start = connection->map->width - s_num_padding_blocks;
					ctb.x_end = connection->map->width;
					s32 mid_point = ((num_blocks_height / 2) + connection->offset);
					ctb.y_start = mid_point - (num_blocks_height / 2) - s_num_padding_blocks;
					ctb.y_end = mid_point + (num_blocks_height / 2) + s_num_padding_blocks;
					ctb.y_start = rsl::clamp_min(ctb.y_start, 0);
					ctb.y_end = rsl::clamp_max(ctb.y_end, connection->map->height);

					x = 0;
					y = mid_point - ctb.y_start;
				}

				for (s32 conn_y = ctb.y_start; conn_y < ctb.y_end; ++conn_y)
				{
					for (s32 conn_x = ctb.x_start; conn_x < ctb.x_end; ++conn_x)
					{
						s32 tile_idx = connection->map->tile_index(conn_x, conn_y);
						assign_tile(x, y, tile_idx);
					}
				}
			}
		}

	public:
		MapTexture(const MapHeader* mapHeader)
		{
			m_width = mapHeader->width + (2 * s_num_padding_blocks);
			m_height = mapHeader->height + (2 * s_num_padding_blocks);
			m_block_ids = rsl::make_unique<s8[]>(m_width * m_height);
			
			fill_border_block(mapHeader->border_block_idx);
			fill_inner_map(mapHeader->map_blocks, mapHeader->blockset);
			fill_connections(mapHeader->connections);

			MapBlocks* map_blocks = load_map_blocks(mapHeader->map_blocks);
			BlockSet* blockset = load_blockset(mapHeader->blockset);

			// Fill in the inner map
			rsl::unique_array<Block> map_blocks;
			for (s32 block_idx = 0; block_idx < map_blocks.count(); ++block_idx)
			{
				// Calculate the coordinate of the block on the texture
				s32 block_x_coord = block_idx % num_blocks_width;
				s32 block_y_coord = block_idx % num_blocks_height;

				s32 abs_block_x_coord = block_x_coord + s_num_padding_blocks;
				s32 abs_block_y_coord = block_y_coord + s_num_padding_blocks;

				const Block& block = map_blocks[block_idx];
				for (s32 y = 0; y < Block::height_tiles(); ++y)
				{
					for (s32 x = 0; x < Block::width_tiles(); ++x)
					{
						s32 tile_idx = y * Block::width_tiles() + x;
						s32 tile_x_coord = abs_block_x_coord + x;
						s32 tile_y_coord = abs_block_y_coord + y;

						rsl::byte block_tile_index = block.indices()[tile_idx];
						assign_tile(tile_x_coord, tile_y_coord, block_tile_index);
					}
				}
			}

			// Fill in the connections

			if (jsonBlob.contains("connections"))
			{
				for (const auto& connection_json : jsonBlob["connections"])
				{
					// A connection is always positions at the middle of the current map and we use the offset
					// to move it left or right (for north/south connections) or up or down (for east/west connections)
					// This connection value is in square (2x2 tiles) 

					rsl::unique_ptr<MapConnection> connection = rsl::make_unique<MapConnection>();
					connection->direction = rsl::enum_refl::enum_cast<Direction>(connection_json["direction"].get<rsl::string_view>()).value();
					connection->map = connection_json["map"];
					connection->offset = connection_json["offset"]; // is in squares (2x2 tiles)

					// Calculate the band of tiles of the connection we have to draw
					struct ConnectionTileBand
					{
						s32 x_start;
						s32 x_end;
						s32 y_start;
						s32 y_end;
					};

					s32 x = 0;
					s32 y = 0;
					s32 conn_map_x = 0;
					s32 conn_map_y = 0;
					ConnectionTileBand ctb{};
					switch (connection->direction)
					{
					case pokemon::Direction::North:
						ctb.y_start = connection->map->height - s_num_padding_blocks;
						ctb.y_end = connection->map->height;
						s32 mid_point = ((num_blocks_width / 2) + connection->offset);
						ctb.x_start = mid_point - (num_blocks_width / 2) - s_num_padding_blocks;
						ctb.x_end = mid_point + (num_blocks_width / 2) + s_num_padding_blocks;
						ctb.x_start = rsl::clamp_min(ctb.x_start, 0);
						ctb.x_end = rsl::clamp_max(ctb.x_end, connection->map->width);

						y = 0;
						x = mid_point - ctb.x_start;
					case pokemon::Direction::East:
						ctb.x_start = 0;
						ctb.x_end = s_num_padding_blocks;
						s32 mid_point = ((num_blocks_height / 2) + connection->offset);
						ctb.y_start = mid_point - (num_blocks_height / 2) - s_num_padding_blocks;
						ctb.y_end = mid_point + (num_blocks_height / 2) + s_num_padding_blocks;
						ctb.y_start = rsl::clamp_min(ctb.y_start, 0);
						ctb.y_end = rsl::clamp_max(ctb.y_end, connection->map->height);

						x = total_blocks_width - s_num_padding_blocks;
						y = mid_point - ctb.y_start;
					case pokemon::Direction::South:
						ctb.y_start = 0;
						ctb.y_end = s_num_padding_blocks;
						s32 mid_point = ((num_blocks_width / 2) + connection->offset);
						ctb.x_start = mid_point - (num_blocks_width / 2) - s_num_padding_blocks;
						ctb.x_end = mid_point + (num_blocks_width / 2) + s_num_padding_blocks;
						ctb.x_start = rsl::clamp_min(ctb.x_start, 0);
						ctb.x_end = rsl::clamp_max(ctb.x_end, connection->map->width);

						y = total_blocks_height - s_num_padding_blocks;
						x = mid_point - ctb.x_start;
					case pokemon::Direction::West:
						ctb.x_start = connection->map->width - s_num_padding_blocks;
						ctb.x_end = connection->map->width;
						s32 mid_point = ((num_blocks_height / 2) + connection->offset);
						ctb.y_start = mid_point - (num_blocks_height / 2) - s_num_padding_blocks;
						ctb.y_end = mid_point + (num_blocks_height / 2) + s_num_padding_blocks;
						ctb.y_start = rsl::clamp_min(ctb.y_start, 0);
						ctb.y_end = rsl::clamp_max(ctb.y_end, connection->map->height);

						x = 0;
						y = mid_point - ctb.y_start;
					}

					for (s32 conn_y = ctb.y_start; conn_y < ctb.y_end; ++conn_y)
					{
						for (s32 conn_x = ctb.x_start; conn_x < ctb.x_end; ++conn_x)
						{
							s32 tile_idx = connection->map->tile_index(conn_x, conn_y);
							assign_tile(x, y, tile_idx);
						}
					}

					//assign_tile(x, y, tile_idx);


					//rsl::unique_ptr<MapConnection> connection = rsl::make_unique<MapConnection>();
					//connection->direction = rsl::enum_refl::enum_cast<Direction>(connection_json["direction"].get<rsl::string_view>()).value();
					//connection->map = connection_json["map"];
					//connection->offset = connection_json["offset"];
					//switch (connection->direction)
					//{
					//case pokemon::Direction::North:		 m_north_connection = rsl::move(connection); break;
					//case pokemon::Direction::East:		 m_east_connection  = rsl::move(connection); break;
					//case pokemon::Direction::South:		 m_south_connection = rsl::move(connection); break;
					//case pokemon::Direction::West:		 m_west_connection  = rsl::move(connection); break;
					//}
				}
			}
		}

		s32 byte_size() const;
		s32 num_tiles() const;
		s32 num_blocks() const;

	private:
		static constexpr s8 s_num_padding_blocks = 3;
		s8 m_width;
		s8 m_height;
		rsl::unique_array<s8> m_block_ids;
	};

	Map::Map(rex::gfx::Scene* scene, const rex::json::json& jsonBlob)
		: m_player_chunk()
		, m_scene(scene)
	{
		// We can use 3 strategies. 
		// ============================================================
		// Do 5 render calls, 1 for the map and 4 for each connection
		// fill each squad with the texture they require
		// ============================================================
		// Combine all tiles into a single quad and only perform 1 draw call <--- like most performant as everything is static
		// ============================================================
		// Draw each tile seperately as we know which tile it uses of the tileset
		// ============================================================

		MapHeader* map_header = load_map_header(jsonBlob);
		MapTexture map_texture(map_header);

		auto texture_format = rex::gfx::TextureFormat::Unorm4Srgb;

		rsl::unique_array<rsl::byte> map_texture_memory = allocate_map_texture_memory(jsonBlob);
		s32 num_tiles = map_texture_memory.byte_size();
		num_tiles /= rex::gfx::format_byte_size(texture_format); // num bytes -> num pixels
		num_tiles /= 64; // num pixels -> num tiles
		fill_texture_with_padding_tile(jsonBlob, map_texture_memory.get(), map_texture_memory.byte_size() / rex::gfx::format_byte_size(rex::gfx::TextureFormat::Unorm4Srgb));
		fill_texture_with_map(jsonBlob, map_texture_memory.get());
		fill_texture_with_connections(jsonBlob, map_texture_memory.get());

		m_name.assign(jsonBlob["name"].get<rsl::string_view>());

		m_tileset = load_tileset(jsonBlob["tileset"]);
		m_blockset = load_blockset(jsonBlob["blockset"]);
		m_blocks = load_blocks(jsonBlob["blocks"]);
		m_width = jsonBlob["width"];
		m_height = jsonBlob["height"];
	}

	void Map::fill_texture_with_padding_tile(const rex::json::json& jsonBlob, rsl::byte* textureMemory, s32 numTiles)
	{
		s32 padding_tile_idx = jsonBlob["padding_tile"];

		const rsl::string_view blk_header = jsonBlob["block_header"];
		rex::json::json blk_json = rex::json::read_from_file(blk_header);
		rsl::string_view tileset_filepath = blk_json["tileset"];
		const rsl::unique_ptr<rsl::byte> tileset = load_tileset(tileset_filepath);
		rsl::fill_n(textureMemory, numTiles, padding_tile_idx);
	}

	void Map::fill_texture_with_connections(const rex::json::json& jsonBlob, rsl::byte* textureMemory)
	{
		if (!jsonBlob.contains("connections"))
		{
			return; // This map has no connections, often true for interiors
		}

		for (const auto& connection_json : jsonBlob["connections"])
		{
			rsl::unique_ptr<MapConnection> connection = rsl::make_unique<MapConnection>();
			connection->direction = rsl::enum_refl::enum_cast<Direction>(connection_json["direction"].get<rsl::string_view>()).value();
			connection->map = connection_json["map"];
			connection->offset = connection_json["offset"];
			switch (connection->direction)
			{
			case pokemon::Direction::North:		 m_north_connection = rsl::move(connection); break;
			case pokemon::Direction::East:		 m_east_connection = rsl::move(connection); break;
			case pokemon::Direction::South:		 m_south_connection = rsl::move(connection); break;
			case pokemon::Direction::West:		 m_west_connection = rsl::move(connection); break;
			}
		}
	}

	MapConnection* Map::north_connection()
	{
		return m_north_connection.get();
	}
	MapConnection* Map::east_connection()
	{
		return m_east_connection.get();
	}
	MapConnection* Map::south_connection()
	{
		return m_south_connection.get();
	}
	MapConnection* Map::west_connection()
	{
		return m_west_connection.get();
	}

	void Map::set_player_chunk(rsl::string_view name)
	{
		// Don't load to a chunk if it's already the active chunk of the player
		if (is_current_player_chunk(name))
		{
			return;
		}

		// If we're in a chunk already, we need to clear all active chunks
		// We might be able to optimize here by only deactiviating the chunks
		// that won't be active when the new chunk is active
		if (m_player_chunk.handle() != entt::null)
		{
			m_active_chunk_components.clear();
		}

		rex::gfx::Entity entity = m_scene->find_entity(name);
		REX_ASSERT_X(entity.handle() != entt::null, "Loading a chunk that is not in the scene. chunk name: {}", name);

		m_player_chunk = entity;
		ChunkComponent* player_chunk = m_active_chunk_components.emplace_back(&m_player_chunk.component<ChunkComponent>());
		for (const ChunkConnection& conn : player_chunk->connections())
		{
			rex::gfx::Entity conn_ent = m_scene->find_entity(conn.dst_chunk_name());
			m_active_chunk_components.push_back(&conn_ent.component<ChunkComponent>());
		}

		// build texture based on tileset and blocket.
		// ..
		// The tileset and blockset is shared for the entire overworld
		// So there's no need for it to actually be stored in the chunk as all this data will be duplicated
		// There are no non-overworld chunks that connect with each other

		//TileSet* tileset = load_tileset(m_player_chunk->tileset());
		//BlockSet* blockset = load_blockset(m_player_chunk->blockset());
		//Blocks* blocks = load_blocks(m_player_chunk->blocks());

	}

	void Map::load_all_tilesets(rsl::string_view tilesetsPath)
	{
		rsl::vector<rsl::string> files;
		rex::memory::Blob file_blob = rex::vfs::read_file(tilesetsPath);
		rsl::string_view file_content = rex::memory::blob_to_string_view(file_blob);
		rsl::vector<rsl::string_view> lines = rsl::split(file_content, "\n");
		for (rsl::string_view line : lines)
		{
			if (!line.empty())
			{
				line = rex::strip(line);
				rsl::string tileset_path = rex::path::join(rex::vfs::mount_path(rex::MountingPoint::Tilesets), line);
				m_tilesets.push_back(load_tileset(tileset_path));
			}
		}
	}

	void Map::load_all_chunks(const rex::json::json& jsonBlob)
	{
		if (!jsonBlob.contains("chunks"))
		{
			return;
		}
		const rex::json::json& all_chunks_json = jsonBlob["chunks"];

		for (const rex::json::json& chunk_json : all_chunks_json)
		{
			rex::gfx::Entity& entity = m_chunks.emplace_back(m_scene->add_entity(chunk_json["name"]));
			entity.add_component<ChunkComponent>(chunk_json);
		}
	}
	bool Map::is_current_player_chunk(rsl::string_view name) const
	{
		if (m_player_chunk.handle() == entt::null)
		{
			return false;
		}

		if (m_player_chunk.component<ChunkComponent>().name() != name)
		{
			return false;
		}

		return true;
	}

	rsl::unique_array<rsl::byte> Map::allocate_map_texture_memory(const rex::json::json& jsonBlob)
	{
		// The calculation for the memory required for the map is pretty simple
		// we load the width and height of the map, and add 3 blocks in padding in every direction.
		s32 padding_depth = 3;
		s32 width_with_padding = jsonBlob["width"] + padding_depth;
		s32 height_with_padding = jsonBlob["height"] + padding_depth;
		s32 memory_size_required = width_with_padding * height_with_padding * rex::gfx::format_byte_size(rex::gfx::TextureFormat::Unorm4Srgb);

		return rsl::make_unique<rsl::byte[]>(memory_size_required);
	}

	void Map::fill_texture_with_map(const rex::json::json& jsonBlob, rsl::byte* textureMemory)
	{
		const rsl::string_view blk_file = jsonBlob["map_blocks"];
		const rsl::string_view blk_set_filepath = jsonBlob["block_set"];
		const rsl::string_view blk_header = jsonBlob["block_header"];

		// Load the blockset into memory
		REX_ASSERT_X(rex::file::exists(blk_set_filepath), "The blockset of {} does not exists. blk set file: {}", rex::quoted(blk_file), rex::quoted(blk_set));
		rex::json::json blk_json = rex::json::read_from_file(blk_header);
		if (blk_json.is_discarded())
		{
			REX_ERROR(LogMap, "Failed to load block header. JSON parsing error. filepath: {}", blk_header);
			return;
		}

		rsl::string_view tileset_filepath = blk_json["tileset"];

		// Now that know all the files that we need to load, let's load all of them in memory
		const rsl::unique_ptr<rsl::byte> blocks = load_blocks(blk_file);
		const rsl::unique_ptr<rsl::byte> block_set = load_block_set(blk_set_filepath);
		const rsl::unique_ptr<rsl::byte> tileset = load_tileset(tileset_filepath);

		// Now let's build the texture from the above data
		// Remember, the blocks are going left to right, up to down.
		// Blocks are just indices into the blockset, which in turn are indices into the tileset

		// Allocte a texturre that's big enough to hold the entire texture we want to render

		rsl::unique_array<rsl::byte> map_texture_data = rsl::make_unique<rsl::byte[]>(width * height * rex::gfx::format_byte_size(rex::gfx::TextureFormat::Unorm4Srgb));

		// Now start filling in the texture content based on the files we loaded above
		s32 x_offset_in_texture = 0;
		s32 y_offset_in_texture = 0;
		s32 num_blocks_x = jsonBlob["width"];
		s32 num_blocks_y = jsonBlob["height"];

		// The pixel data is stored left to right, top to down, but splits up for every block
		// This means that the first block would fill in the top left data in a block of 32x32 pixels
		// the second would fill in the data to the right of this block and fill in a block of 32x32 pixels
		// This layout is not what native texture layout expects it to be, meaning we have to work around this

		// We go over the entire map and jump in size of blocks
		for (s32 y = 0; y < num_blocks_y; ++y)
		{
			for (s32 x = 0; x < num_blocks_x; ++x)
			{
				// For each iteration, we retrieve the block index we're currently looking at and load it from the map blocks
				s32 current_block_idx = y * num_blocks_x + x;

				// This index is just an index into the block set, so we need to load the actual block of the block set
				s32 block_idx_to_copy = blocks[current_block_idx];
				const Block& block_to_copy = block_set[block_idx_to_copy];

				// now go over each tile that'd be in a block
				for (s32 tile_y = 0; tile_y < Block::num_tiles_y(); ++tile_y)
				{
					for (s32 tile_x = 0; tile_x < Block::num_tiles_x(); ++tile_x)
					{
						// For each iteration, we retrieve the tile index we're currently looking at and load it from the tileset
						s32 current_tile_idx_in_block = tile_y * Block::num_tiles_y() + tile_x;
						s32 tile_idx = block_to_copy.index_at(current_tile_idx_in_block);
						const Tile& tile = tileset[tile_idx];

						// Once we have the tile, we need to map this onto the texture where it would be located
						s32 offset_in_texture_x = x * Block::width_px() + tile_x;
						s32 offset_in_texture_y = y * Block::heiht_px() + tile_y;
						s32 offset_in_texture = offset_in_texture_y * width + offset_in_texture_x;

						// When we found this area, we copy the tile data into the data that we'll use to initialize the texture
						rsl::memcpy(map_texture_data.get() + offset_in_texture, tile.data(), tile.size());
					}
				}
			}
		}

		rsl::unique_ptr<Texture2D> map_texture = rex::rhi::allocate_texture_2d(width, height, rex::gfx::TextureFormat::Unorm4Srgb);
		rex::gfx::copy_to_texture(map_texture, map_texture_data.get(), map_texture_data.byte_size());
	}

}
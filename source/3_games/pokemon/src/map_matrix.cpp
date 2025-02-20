#include "pokemon/map_matrix.h"

#include "pokemon/blockset.h"
#include "pokemon/poke_map.h"
#include "pokemon/assets/database.h"
#include "pokemon/map_coordinates.h"

#include "pokemon/render_constants.h"

#include "rex_engine/shapes/rect.h"

namespace pokemon
{
  // Returns the rex::Rect8 in which the connection map should be drawn
  rex::Rect8 rect_for_connection(const MapData* map, const MapConnection& conn)
  {
    // Maps are always center aligned, their midpoint shifts depending on the connection offset
    s8 mid_point_x = constants::g_map_padding_blocks + ((map->map_header.width / 2) + conn.offset);
    s8 mid_point_y = constants::g_map_padding_blocks + ((map->map_header.height / 2) + conn.offset);

    s8 start = 0;
    s8 end = 0;
    // The formula to calculate the start is the same for each direction, just takes different input
    // depending on the connection's direction
    if (conn.direction == Direction::North || conn.direction == Direction::South)
    {
      start = static_cast<s8>(rsl::clamp_min(mid_point_x - (conn.map->map_header.width / 2) + conn.offset, 0));
      s8 depth = static_cast<s8>(rsl::min(map->map_header.width + constants::g_map_padding_blocks, static_cast<s32>(conn.map->map_header.width)));
      end = static_cast<s8>(start + depth);
    }
    else
    {
      start = static_cast<s8>(rsl::clamp_min(mid_point_y - (conn.map->map_header.height / 2) + conn.offset, 0));
      s8 depth = static_cast<s8>(rsl::min(map->map_header.height + constants::g_map_padding_blocks, static_cast<s32>(conn.map->map_header.height)));
      end = static_cast<s8>(start + depth);
    }

    // Fill in the rex::Rect8 based on the direction of the connection
    rex::Rect8 res{};

    switch (conn.direction)
    {
    case Direction::North:
      res.top_left.x = start;
      res.top_left.y = 0;
      res.bottom_right.x = end;
      res.bottom_right.y = res.top_left.y + constants::g_map_padding_blocks;
      break;
    case Direction::East:
      res.top_left.x = map->map_header.width + constants::g_map_padding_blocks;
      res.top_left.y = start;
      res.bottom_right.x = res.top_left.x + constants::g_map_padding_blocks;
      res.bottom_right.y = end;
      break;
    case Direction::South:
      res.top_left.x = start;
      res.top_left.y = map->map_header.height + constants::g_map_padding_blocks;
      res.bottom_right.x = end;
      res.bottom_right.y = res.top_left.y + constants::g_map_padding_blocks;
      break;
    case Direction::West:
      res.top_left.x = 0;
      res.top_left.y = start;
      res.bottom_right.x = res.top_left.x + constants::g_map_padding_blocks;
      res.bottom_right.y = end;
      break;
    }

    return res;
  }

  rsl::pointi8 project_point_to_conn(const MapData* mapObject, const MapConnection& conn, rsl::pointi8 coord)
  {
    s8 projected_point = 0;
    if (conn.direction == Direction::North || conn.direction == Direction::South)
    {
      s8 mid_point_x = conn.map->map_header.width / 2 - conn.offset;
      s8 offset_from_mid = constants::g_map_padding_blocks + (mapObject->map_header.width / 2) - coord.x;
      projected_point = mid_point_x - offset_from_mid;
    }
    else
    {
      s8 mid_point_y = conn.map->map_header.height / 2 - conn.offset;
      s8 offset_from_mid = constants::g_map_padding_blocks + (mapObject->map_header.height / 2) - coord.y;
      projected_point = mid_point_y - offset_from_mid;
    }

    rsl::pointi8 res{};

    switch (conn.direction)
    {
    case Direction::North:
      res.x = projected_point;
      res.y = conn.map->map_header.height - constants::g_map_padding_blocks;
      break;
    case Direction::East:
      res.x = 0;
      res.y = projected_point;
      break;
    case Direction::South:
      res.x = projected_point;
      res.y = 0;
      break;
    case Direction::West:
      res.x = conn.map->map_header.width - constants::g_map_padding_blocks;
      res.y = projected_point;
      break;
    }

    return res;
  }

  MapMatrix::MapMatrix(const MapData* mapObject)
  {
    m_total_width = mapObject->map_header.width + (2 * constants::g_map_padding_blocks);
    m_total_height = mapObject->map_header.height + (2 * constants::g_map_padding_blocks);

    m_block_indices = rsl::make_unique<u8[]>(m_total_width * m_total_height);
    init_blocks(mapObject->map_header.border_block_idx);

    fill_connections(mapObject);
    fill_inner_map(mapObject);
  }

  s32 MapMatrix::width_in_blocks() const
  {
    return m_total_width;
  }
  s32 MapMatrix::height_in_blocks() const
  {
    return m_total_height;
  }
  s32 MapMatrix::major_dimension_in_blocks() const
  {
    return width_in_blocks();
  }
  s32 MapMatrix::width_in_tiles() const
  {
    return width_in_blocks() * Block::num_tiles_per_row();
  }
  s32 MapMatrix::height_in_tiles() const
  {
    return height_in_blocks() * Block::num_tiles_per_column();
  }
  s32 MapMatrix::major_dimension_in_tiles() const
  {
    return width_in_blocks() * Block::num_tiles_per_row();
  }

  s32 MapMatrix::index_at(BlockCoord blockCoord) const
  {
    s32 idx = coords::coord_to_index(blockCoord, m_total_width);
    return m_block_indices[idx];
  }
  s32 MapMatrix::index_at(TileCoord tileCoord) const
  {
    BlockCoord block_coord = coords::tile_to_block_coord(tileCoord);
    return index_at(block_coord);
  }

  void MapMatrix::init_blocks(s8 borderBlockIdx)
  {
    rsl::fill_n(m_block_indices.begin(), m_block_indices.count(), borderBlockIdx);
  }
  void MapMatrix::fill_connections(const MapData* mapObject)
  {
    for (const MapConnection& conn : mapObject->connections)
    {
      // Calculate the rect in which the connection will be drawn
      // This rect covers the entire padding region
      rex::Rect8 rect = rect_for_connection(mapObject, conn);

      // Calculate the top left position where the drawing will start, inside of this rect
      rsl::pointi8 top_left_conn = project_point_to_conn(mapObject, conn, rect.top_left);

      // Load the map blocks of the connection so we can assign the right block index to the map matrix
      const rsl::unique_array<u8>& conn_map_blocks = asset_db::find_map_blocks(conn.map->map_header);

      // Go over the blocks of the connection and assign the block index to the map matrix
      for (s8 y = rect.top_left.y, conn_y = top_left_conn.y; y < rect.bottom_right.y; ++y, ++conn_y)
      {
        for (s8 x = rect.top_left.x, conn_x = top_left_conn.x; x < rect.bottom_right.x; ++x, ++conn_x)
        {
          // Look up the block index of the current block we're looking at in the connection map
          s16 conn_idx = conn_y * conn.map->map_header.width + conn_x;
          u8 block_idx = conn_map_blocks[conn_idx];

          // Assign the connection's block index to the map matrix
          s16 index = (y * m_total_width) + x;
          m_block_indices[index] = block_idx;
        }
      }
    }
  }
  void MapMatrix::fill_inner_map(const MapData* mapObject)
  {
    s32 height = mapObject->map_header.height;
    s32 width = mapObject->map_header.width;

    const rsl::unique_array<u8>& map_blocks = asset_db::find_map_blocks(mapObject->map_header);
    for (s8 y = 0; y < height; ++y)
    {
      for (s8 x = 0; x < width; ++x)
      {
        s32 index = y * width + x;
        s16 map_matrix_index = ((y + constants::g_map_padding_blocks) * m_total_width) + (x + constants::g_map_padding_blocks);
        m_block_indices[map_matrix_index] = map_blocks[index++];
      }
    }
  }

}
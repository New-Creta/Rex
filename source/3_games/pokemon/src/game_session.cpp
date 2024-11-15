#include "pokemon/game_session.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_engine/text_processing/json.h"
#include "rex_engine/text_processing/text_processing.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"

#include "pokemon/poke_structs.h"
#include "rex_std/bonus/math.h"
#include "rex_std/bonus/math/point.h"

#include "rex_std/unordered_map.h"

#include "rex_engine/gfx/system/rhi.h"
#include "rex_engine/images/stb_image.h"

#include "pokemon/block.h"
#include "pokemon/blockset.h"
#include "pokemon/tileset.h"

#include "rex_engine/profiling/instrumentor.h"

namespace pokemon
{
  DEFINE_LOG_CATEGORY(LogGameSession);

  rsl::unordered_map<rsl::string_view, rsl::unique_ptr<MapData>> g_maps;
  rsl::unordered_map<rsl::string_view, rsl::shared_ptr<BlockSet>> g_map_blocks;

  MapData* find_map(rsl::string_view mapPath);
  MapData* find_map_without_connections(rsl::string_view mapPath);

  MapHeader load_map_header(const rex::json::json& jsonBlob);
  MapHeader load_map_header(rsl::string_view mapPath);

  void load_map_data(MapData* mapObject, const rex::json::json& jsonBlob);
  rsl::unique_ptr<MapData> load_map_data(rsl::string_view mapPath);
  rsl::unique_ptr<MapData> load_map_header_only(rsl::string_view mapPath);

  const BlockSet* find_map_blocks(rsl::string_view mapBlockFilePath)
  {
    if (!g_map_blocks.contains(mapBlockFilePath))
    {
      MapData* map_data = find_map(mapBlockFilePath);
      return g_map_blocks.emplace(mapBlockFilePath, rsl::move(map_data)).inserted_element->value.get();
    }

    return g_map_blocks.at(mapBlockFilePath).get();
  }

  MapData* find_map(rsl::string_view mapPath)
  {
    if (!g_maps.contains(mapPath))
    {
      rsl::unique_ptr<MapData> map_data = load_map_data(mapPath);
      
      // If the map wasn't fully loaded (eg. it was loaded as a connection)
      // We need to load it fully now
      if (!map_data->fully_loaded)
      {
        rex::memory::Blob file_content = rex::vfs::read_file(mapPath);
        rex::json::json json_blob = rex::json::parse(file_content);
        load_map_data(map_data.get(), json_blob);
      }
      return g_maps.emplace(mapPath, rsl::move(map_data)).inserted_element->value.get();
    }

    return g_maps.at(mapPath).get();
  }
  MapData* find_map_without_connections(rsl::string_view mapPath)
  {
    if (!g_maps.contains(mapPath))
    {
      rsl::unique_ptr<MapData> map_object = load_map_header_only(mapPath);
      map_object->fully_loaded = false;
      return g_maps.emplace(mapPath, rsl::move(map_object)).inserted_element->value.get();
    }

    return g_maps.at(mapPath).get();
  }

  MapHeader load_map_header(const rex::json::json& jsonBlob)
  {
    MapHeader map_header{};

    map_header.name = jsonBlob["name"];
    map_header.map_blocks = jsonBlob["map_blocks"];
    map_header.blockset = jsonBlob["blockset"];
    map_header.width = jsonBlob["width"];
    map_header.height = jsonBlob["height"];
    map_header.border_block_idx = jsonBlob["border_block_idx"];

    return map_header;
  }
  MapHeader load_map_header(rsl::string_view mapPath)
  {
    MapHeader map_header{};

    rex::memory::Blob file_content = rex::vfs::read_file(mapPath);
    rex::json::json json_blob = rex::json::parse(file_content);
    
    return load_map_header(json_blob);
  }

  void load_map_data(MapData* mapObject, const rex::json::json& jsonBlob)
  {
    // Connections
    if (jsonBlob.contains("connections"))
    {
      mapObject->connections.reserve(jsonBlob["connections"].size());
      for (const auto& connection_json : jsonBlob["connections"])
      {
        MapConnection& connection = mapObject->connections.emplace_back();
        connection.direction = rsl::enum_refl::enum_cast<Direction>(connection_json["direction"].get<rsl::string_view>()).value();
        connection.map = find_map_without_connections(connection_json["map"]);
        connection.offset = connection_json["offset"]; // is in squares (2x2 tiles)
      }
    }

    // Wrap events
    if (jsonBlob.contains("wraps"))
    {
      mapObject->wrap_events.reserve(jsonBlob["wraps"].size());
      for (const auto& wrap_json : jsonBlob["wraps"])
      {
        WrapEvent& wrap_event = mapObject->wrap_events.emplace_back();
        wrap_event.x = wrap_json["x"];
        wrap_event.y = wrap_json["y"];
        wrap_event.dst_map_id = wrap_json["dst_map_id"];
        wrap_event.dst_warp_id = wrap_json["dst_warp_id"];
      }
    }

    mapObject->fully_loaded = true;
  }

  rsl::unique_ptr<MapData> load_map_data(rsl::string_view mapPath)
  {
    rex::json::json json_blob = rex::json::read_from_file(mapPath);

    rsl::unique_ptr<MapData> map_data = rsl::make_unique<MapData>();
    map_data->map_header = load_map_header(json_blob);

    load_map_data(map_data.get(), json_blob);

    return map_data;
  }
  rsl::unique_ptr<MapData> load_map_header_only(rsl::string_view mapPath)
  {
    rex::memory::Blob file_content = rex::vfs::read_file(mapPath);
    rex::json::json json_blob = rex::json::parse(file_content);

    REX_ASSERT_X(!json_blob.is_discarded(), "Invalid map json file. {}", rex::quoted(mapPath))
    rsl::unique_ptr<MapData> map_object = rsl::make_unique<MapData>();
    map_object->map_header = load_map_header(json_blob);

    return map_object;
  }

  struct rect
  {
    rsl::pointi8 top_left;
    rsl::pointi8 bottom_right;
  };

  void fill_rect_with_block_idx(s8* array, rect rect, s8 width, s8 blockIdx)
  {
    for (s8 y = rect.top_left.y; y < rect.bottom_right.y; ++y)
    {
      for (s8 x = rect.top_left.x; x < rect.bottom_right.x; ++x)
      {
        s16 index = (y * width) + x;
        array[index] = blockIdx;
      }
    }
  }



  namespace map_matrix
  {
    s8 g_padding = 3;

    // Returns the rect in which a connection, specified by a direction, will populate its blocks in
    rect rect_for_direction(const MapData* mapObject, Direction direction)
    {
      // The width/height of the map plane is the width/height of the map + padding on each side
      s8 total_width = mapObject->map_header.width + (2 * g_padding);
      s8 total_height = mapObject->map_header.height + (2 * g_padding);

      rect res{};
      
      // The rect of a direction corresponds to the full length of the edge of the map plane
      // going 3 blocks deep into the map
      // NORTH - the top 3 rows of the map plane
      // EAST - the right 3 columns of the map plane
      // SOUTH - the bottom 3 rows of the map plane
      // WEST - the left 3 columns of the map plane

      switch (direction)
      {
      case pokemon::Direction::North:
        res.top_left.x = 0;
        res.top_left.y = 0;
        res.bottom_right.x = total_width;
        res.bottom_right.y = g_padding;
        break;
      case pokemon::Direction::East:
        res.top_left.x = total_width - g_padding;
        res.top_left.y = 0;
        res.bottom_right.x = total_width;
        res.bottom_right.y = total_height;
        break;
      case pokemon::Direction::South:
        res.top_left.x = 0;
        res.top_left.y = total_height - g_padding;
        res.bottom_right.x = total_width;
        res.bottom_right.y = total_height;
        break;
      case pokemon::Direction::West:
        res.top_left.x = 0;
        res.top_left.y = 0;
        res.bottom_right.x = g_padding;
        res.bottom_right.y = total_height;
        break;
      }

      return res;
    }

    // Returns the rect in which the connection map should be drawn
    rect rect_for_connection(const MapData* map, const MapConnection& conn)
    {
      // Maps are always center aligned, their midpoint shifts depending on the connection offset
      s8 mid_point_x = g_padding + ((map->map_header.width / 2) + conn.offset);
      s8 mid_point_y = g_padding + ((map->map_header.height / 2) + conn.offset);

      s8 start = 0;
      s8 end = 0;
      // The formula to calculate the start is the same for each direction, just takes different input
      // depending on the connection's direction
      if (conn.direction == Direction::North || conn.direction == Direction::South)
      {
        start = rsl::clamp_min(mid_point_x - (conn.map->map_header.width / 2) + conn.offset, 0);
        end = start + rsl::min(map->map_header.width + g_padding, static_cast<s32>(conn.map->map_header.width));
      }
      else
      {
        start = rsl::clamp_min(mid_point_y - (conn.map->map_header.height / 2) + conn.offset, 0);
        end = start + rsl::min(map->map_header.height + g_padding, static_cast<s32>(conn.map->map_header.height));
      }

      // Fill in the rect based on the direction of the connection
      rect res{};

      switch (conn.direction)
      {
      case Direction::North:
        res.top_left.x = start;
        res.top_left.y = 0;
        res.bottom_right.x = end;
        res.bottom_right.y = res.top_left.y + g_padding;
        break;
      case Direction::East:
        res.top_left.x = map->map_header.width + g_padding;
        res.top_left.y = start;
        res.bottom_right.x = res.top_left.x + g_padding;
        res.bottom_right.y = end;
        break;
      case Direction::South:
        res.top_left.x = start;
        res.top_left.y = map->map_header.height + g_padding;
        res.bottom_right.x = end;
        res.bottom_right.y = res.top_left.y + g_padding;
        break;
      case Direction::West:
        res.top_left.x = 0;
        res.top_left.y = start;
        res.bottom_right.x = res.top_left.x + g_padding;
        res.bottom_right.y = end;
        break;
      }

      return res;
    }

    rsl::pointi8 map_to_abs(rsl::pointi8 coord)
    {
      rsl::pointi8 res{};

      res.x = coord.x + g_padding;
      res.y = coord.y + g_padding;

      return res;
    }
    rsl::pointi8 abs_to_map(rsl::pointi8 coord)
    {
      rsl::pointi8 res{};

      res.x = coord.x - g_padding;
      res.y = coord.y - g_padding;

      return res;
    }

    rsl::pointi8 project_point_to_conn(const MapData* mapObject, const MapConnection& conn, rsl::pointi8 coord)
    {
      s8 conn_midpoint_x = conn.map->map_header.width / 2;
      s8 conn_midpoint_y = conn.map->map_header.height / 2;

      s8 projected_point = 0;
      if (conn.direction == Direction::North || conn.direction == Direction::South)
      {
        s8 mid_point_x = conn.map->map_header.width / 2 - conn.offset;
        s8 offset_from_mid = g_padding + (mapObject->map_header.width / 2) - coord.x;
        projected_point = mid_point_x - offset_from_mid;
      }
      else
      {
        s8 mid_point_y = conn.map->map_header.height / 2 - conn.offset;
        s8 offset_from_mid = g_padding + (mapObject->map_header.height / 2) - coord.y;
        projected_point = mid_point_y - offset_from_mid;
      }

      rsl::pointi8 res{};

      switch (conn.direction)
      {
      case Direction::North:
        res.x = projected_point;
        res.y = conn.map->map_header.height - g_padding;
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
        res.x = conn.map->map_header.width - g_padding;
        res.y = projected_point;
        break;
      }

      return res;
    }

    class MapMatrix
    {
    public:
      MapMatrix(const MapData* mapObject)
      {
        m_total_width = mapObject->map_header.width + (2 * g_padding);
        m_total_height = mapObject->map_header.height + (2 * g_padding);

        m_blocks = rsl::make_unique<Block[]>(m_total_width * m_total_height);
        init_blocks(mapObject->map_header.border_block_idx);
        
        fill_connections(mapObject);
        fill_inner_map(mapObject);
      }

      s8 width() const
      {
        return m_total_width;
      }
      s8 height() const
      {
        return m_total_height;
      }

      const Block& index_at(s8 x, s8 y) const
      {
        return m_blocks[y * m_total_width + x];
      }

    private:
      void init_blocks(s8 borderBlockIdx)
      {
        for (Block& block : m_blocks)
        {
          block.set(borderBlockIdx);
        }
      }
      void fill_connections(const MapData* mapObject)
      {
        for (const MapConnection& conn : mapObject->connections)
        {
          // Calculate the rect in which the connection will be drawn
          // This rect covers the entire padding region
          rect rect = rect_for_connection(mapObject, conn);

          // Calculate the top left position where the drawing will start, inside of this rect
          rsl::pointi8 top_left_conn = project_point_to_conn(mapObject, conn, rect.top_left);

          // Load the map blocks of the connection so we can assign the right block index to the map matrix
          const BlockSet* conn_map_blocks = find_map_blocks(conn.map->map_header.map_blocks);

          // Go over the blocks of the connection and assign the block index to the map matrix
          for (s8 y = rect.top_left.y, conn_y = top_left_conn.y; y < rect.bottom_right.y; ++y, ++conn_y)
          {
            for (s8 x = rect.top_left.x, conn_x = top_left_conn.x; x < rect.bottom_right.x; ++x, ++conn_x)
            {
              s16 conn_idx = conn_y * conn.map->map_header.width + conn_x;
              const Block& block = conn_map_blocks->block(conn_idx);

              s16 index = (y * m_total_width) + x;
              m_blocks[index].set(block);
            }
          }
        }
      }
      void fill_inner_map(const MapData* mapObject)
      {
        s16 index = 0;
        const BlockSet* map_blocks = find_map_blocks(mapObject->map_header.map_blocks);
        for (s8 y = g_padding; y < m_total_height - g_padding; ++y)
        {
          for (s8 x = g_padding; x < m_total_width - g_padding; ++x)
          {
            s16 map_matrix_index = y * m_total_width + x;
            m_blocks[map_matrix_index].set(map_blocks->block(index++));
          }
        }
      }


    private:
      rsl::unique_array<Block> m_blocks;
      s8 m_total_width;
      s8 m_total_height;
    };
  }

  //class MapPlane
  //{
  //public:
  //  MapPlane(const MapData* mapObject, const map_matrix::MapMatrix& mapMatrix)
  //  {
  //    s8 block_width_px = 8 * 4; // 4 * tile width
  //    s8 block_height_px = 8 * 4; // 4 * tile height

  //    s16 width = mapMatrix.width() * block_width_px;
  //    s16 height = mapMatrix.height() * block_height_px;

  //    // Load the tileset into memory
  //    s32 tileset_width, tileset_height, num_channels;
  //    rex::memory::Blob file_content = rex::vfs::read_file(mapObject->map_header.blockset);
  //    rex::json::json blockset_json = rex::json::parse(file_content);
  //    rsl::string_view tileset = blockset_json["tileset"];
  //    stbi_uc* tileset_data = stbi_load(tileset.data(), &tileset_width, &tileset_height, &num_channels, 0);

  //    rex::memory::Blob blockset_data = rex::vfs::read_file(blockset_json["blockset"]);
  //    rsl::unique_array<u8> blockset = blockset_data.release_as_array<u8>();

  //    // Allocate the map plane data. The pixel data will copied into this, before its sent to the gpu
  //    m_texture_data = rsl::make_unique<rsl::Rgba[]>(width * height);

  //    // Go over every block and assign the data of the tile to it
  //    const s8 num_indices_per_block = 16;
  //    rsl::array<s8, num_indices_per_block> block_indices;
  //    for (s8 y = 0; y < mapMatrix.height(); ++y)
  //    {
  //      for (s8 x = 0; x < mapMatrix.width(); ++x)
  //      {
  //        s32 blockset_index = mapMatrix.index_at(x, y);
  //        blockset_index *= num_indices_per_block;
  //        rsl::memcpy(block_indices.data(), &blockset[blockset_index], num_indices_per_block);

  //        // Tiles are stored different in the png than the layout expects
  //        // layout expects tiles to be stored row by row, which is technically true
  //        // but you have a lot of padding in between
  //        // 1 byte is used per pixel
  //        for (s8 idx = 0; idx < block_indices.size(); ++idx)
  //        {
  //          s32 tile_idx = block_indices[idx];
  //          s32 tx = tile_idx % 16;
  //          s32 ty = tile_idx / 16;
  //          s32 tile_data_start = 8 * ty * tileset_width + (tx * 8);

  //          for (s8 idx2 = 0; idx2 < 8; ++idx2)
  //          {
  //            stbi_uc* tile_row_data = tileset_data + tile_data_start + (idx2 * tileset_width);

  //            // copy the tile row data into the right place in the texture memory
  //            // first calculate the top left coordinate of the block
  //            s32 block_top_left_offset = y * 32 * width + (x * 32);

  //            // then calculate the top left coordinate of the current tile we're processing
  //            s32 tile_offset_x = idx % 4; // tiles per block row
  //            s32 tile_offset_y = idx / 4; // tiles per block column
  //            s32 tile_top_left_idx = block_top_left_offset + (tile_offset_x * 8) + (tile_offset_y * 8 * width);

  //            // then calculate the position within the tile that we're drawing
  //            s32 tile_pixel_offset_y = idx2;
  //            s32 tile_pixel_offset = tile_top_left_idx + (idx2 * width);

  //            REX_ASSERT_X(m_texture_data.get() + 7 <= m_texture_data.get() + m_texture_data.byte_size(), "Would write outside of texture boundary");
  //            REX_ASSERT_X(tile_row_data + 7 <= tileset_data + (tileset_width * tileset_height * num_channels), "Would read outside of texture boundary");

  //            //rsl::memcpy(m_texture_data.get() + tile_pixel_offset, tile_row_data, 8);
  //            for (s8 i = 0; i < 8; ++i)
  //            {
  //              rsl::Rgba& data = m_texture_data[tile_pixel_offset++];
  //              f32 color = tile_row_data[i];
		//						data.red    = color;
		//						data.green  = color;
		//						data.blue   = color;
  //              data.alpha  = 255;
  //            }
  //          }
  //        }
  //      }
  //    }

  //    // Pass this data over to create the texture
  //    
  //    m_map_texture = rex::gfx::rhi::create_texture2d(width, height, rex::gfx::TextureFormat::Unorm4, m_texture_data.get());
  //  }

  //private:
  //  rsl::unique_array<rsl::Rgba> m_texture_data;
  //  rsl::unique_ptr<rex::gfx::Texture2D> m_map_texture;
  //};

  class Map
  {
  public:
    Map(rsl::unique_ptr<rex::gfx::Texture2D> texture)
      : m_texture(rsl::move(texture))
    {

    }

  private:
    rsl::unique_ptr<rex::gfx::Texture2D> m_texture;
  };

  class MapGenerator
  {
  public:
    Map generate(rsl::string_view filepath)
    {
      rsl::unique_ptr<MapData> map_object = load_map_data(filepath);

      // Create the map matrix, which is a matrix of the map, with the padding, holding a block index in each cell
      map_matrix::MapMatrix map_matrix(map_object.get());

      // Calculate the dimensions of the texture
      s16 width = map_matrix.width() * Block::width_px();
      s16 height = map_matrix.height() * Block::height_px();

      // Allocate the memory to fill in the texture data
      m_texture_data = rsl::make_unique<rsl::Rgba[]>(width * height);

      // Load the tileset and blockset into memory
      rex::json::json blockset_json = rex::json::read_from_file(map_object->map_header.blockset);
      rsl::shared_ptr<BlockSet> blockset = load_blockset(blockset_json["blockset"]);
      rsl::string_view tileset_filepath = blockset_json["tileset"];
      s32 tileset_width, tileset_height, num_channels;
      const u8* tileset_data = stbi_load(tileset_filepath.data(), &tileset_width, &tileset_height, &num_channels, 0);

      // Fill in the texture data
      TileSet tileset(tileset_data);
      fill_texture_data(map_matrix, blockset.get(), tileset);

      rsl::unique_ptr<rex::gfx::Texture2D> map_texture = rex::gfx::rhi::create_texture2d(width, height, rex::gfx::TextureFormat::Unorm4, m_texture_data.get());

      return Map(rsl::move(map_texture));

      //rsl::string_view tileset = blockset_json["tileset"];

      //s32 tileset_width, tileset_height, num_channels;
      //stbi_uc* tileset_data = stbi_load(tileset.data(), &tileset_width, &tileset_height, &num_channels, 0);

      //rex::memory::Blob blockset_data = rex::vfs::read_file(blockset_json["blockset"]);
      //rsl::unique_array<u8> blockset = blockset_data.release_as_array<u8>();

      //// Allocate the map plane data. The pixel data will copied into this, before its sent to the gpu

      //// Go over every block and assign the data of the tile to it
      //const s8 num_indices_per_block = 16;
      //rsl::array<s8, num_indices_per_block> block_indices;
      //for (s8 y = 0; y < mapMatrix.height(); ++y)
      //{
      //  for (s8 x = 0; x < mapMatrix.width(); ++x)
      //  {
      //    s32 blockset_index = mapMatrix.index_at(x, y);
      //    blockset_index *= num_indices_per_block;
      //    rsl::memcpy(block_indices.data(), &blockset[blockset_index], num_indices_per_block);

      //    // Tiles are stored different in the png than the layout expects
      //    // layout expects tiles to be stored row by row, which is technically true
      //    // but you have a lot of padding in between
      //    // 1 byte is used per pixel
      //    for (s8 idx = 0; idx < block_indices.size(); ++idx)
      //    {
      //      s32 tile_idx = block_indices[idx];
      //      s32 tx = tile_idx % 16;
      //      s32 ty = tile_idx / 16;
      //      s32 tile_data_start = 8 * ty * tileset_width + (tx * 8);

      //      for (s8 idx2 = 0; idx2 < 8; ++idx2)
      //      {
      //        stbi_uc* tile_row_data = tileset_data + tile_data_start + (idx2 * tileset_width);

      //        // copy the tile row data into the right place in the texture memory
      //        // first calculate the top left coordinate of the block
      //        s32 block_top_left_offset = y * 32 * width + (x * 32);

      //        // then calculate the top left coordinate of the current tile we're processing
      //        s32 tile_offset_x = idx % 4; // tiles per block row
      //        s32 tile_offset_y = idx / 4; // tiles per block column
      //        s32 tile_top_left_idx = block_top_left_offset + (tile_offset_x * 8) + (tile_offset_y * 8 * width);

      //        // then calculate the position within the tile that we're drawing
      //        s32 tile_pixel_offset_y = idx2;
      //        s32 tile_pixel_offset = tile_top_left_idx + (idx2 * width);

      //        REX_ASSERT_X(texture_data.get() + 7 <= texture_data.get() + texture_data.byte_size(), "Would write outside of texture boundary");
      //        REX_ASSERT_X(tile_row_data + 7 <= tileset_data + (tileset_width * tileset_height * num_channels), "Would read outside of texture boundary");

      //        //rsl::memcpy(m_texture_data.get() + tile_pixel_offset, tile_row_data, 8);
      //        for (s8 i = 0; i < 8; ++i)
      //        {
      //          rsl::Rgba& data = texture_data[tile_pixel_offset++];
      //          f32 color = tile_row_data[i];
      //          data.red = color;
      //          data.green = color;
      //          data.blue = color;
      //          data.alpha = 255;
      //        }
      //      }
      //    }
      //  }
      //}

      //// Pass this data over to create the texture
      //rsl::unique_ptr<rex::gfx::Texture2D> map_texture = rex::gfx::rhi::create_texture2d(width, height, rex::gfx::TextureFormat::Unorm4, texture_data.get());
    }

  private:
    // Calculate the offset inside the texture where the first pixel of the tile is located
    s32 calc_tile_offset_into_texture(s32 tileIdx)
    {
      constexpr s8 pixel_size = 1;
      constexpr s32 tileset_width_px = 128;
      constexpr s8 num_tiles_per_row = 16;

      s32 tx = tileIdx % num_tiles_per_row;
      s32 ty = tileIdx / num_tiles_per_row;
      s32 tile_offset = Tile::height_px() * ty * (tileset_width_px * pixel_size) + (tx * Tile::width_px());

      return tile_offset;
    }

    rsl::Rgba* calc_dst_in_map_texture(rsl::pointi8 mapCoord, s16 mapWidthPx, s8 tileIdxInBlock, s8 tileRow)
    {
      // first calculate the top left coordinate of the block
      s32 block_top_left_offset = mapCoord.y * Block::height_px() * mapWidthPx + (mapCoord.x * Block::width_px());

      // then calculate the top left coordinate of the current tile we're processing
      s32 tile_offset_x = tileIdxInBlock % Block::num_tiles_per_row();
      s32 tile_offset_y = tileIdxInBlock / Block::num_tiles_per_row();
      s32 tile_top_left_idx = block_top_left_offset + (tile_offset_x * 8) + (tile_offset_y * 8 * mapWidthPx);

      // then calculate the position within the tile that we're drawing
      s32 tile_pixel_offset_y = tileRow;
      s32 tile_pixel_offset = tile_top_left_idx + (tileRow * mapWidthPx);

      return m_texture_data.get() + tile_pixel_offset;
    }

    void fill_block(rsl::pointi8 mapCoord, s16 mapWidthPx, const Block& block, const TileSet& tileset)
    {
      for (s8 tile_idx_in_block = 0; tile_idx_in_block < block.num_tiles(); ++tile_idx_in_block)
      {
        u8 tile_idx = block.index_at(tile_idx_in_block);

				// Load each pixel row of the tile
        for (s8 tile_row = 0; tile_row < Tile::height_px(); ++tile_row)
        {
          const u8* tile_row_data_src = tileset.tile_pixel_row(tile_idx, tile_row);
          rsl::Rgba* tile_row_data_dst = calc_dst_in_map_texture(mapCoord, mapWidthPx, tile_idx_in_block, tile_row);
                 
          REX_ASSERT_X(m_texture_data.get() + 7 <= m_texture_data.get() + m_texture_data.byte_size(), "Would write outside of texture boundary");
          //REX_ASSERT_X(tile_row_data_src + 7 <= tileset_data + (tileset_width * tileset_height * num_channels), "Would read outside of texture boundary");

          //rsl::memcpy(m_texture_data.get() + tile_pixel_offset, tile_row_data, 8);
          for (s8 i = 0; i < 8; ++i)
          {
            rsl::Rgba& data = *tile_row_data_dst;
            u8 color = tile_row_data_src[i];
            data.red = color;
            data.green = color;
            data.blue = color;
            data.alpha = 255;
          }

					// Calcualte the offset within the map texture where the pixel row should be copied to

					// Assign the rgba values for every pixel within the row


          //stbi_uc* tile_row_data = tileset_data + tile_data_start + (idx2 * tileset_width);

          //// copy the tile row data into the right place in the texture memory
          //// first calculate the top left coordinate of the block
          //s32 block_top_left_offset = y * 32 * width + (x * 32);

          //// then calculate the top left coordinate of the current tile we're processing
          //s32 tile_offset_x = idx % 4; // tiles per block row
          //s32 tile_offset_y = idx / 4; // tiles per block column
          //s32 tile_top_left_idx = block_top_left_offset + (tile_offset_x * 8) + (tile_offset_y * 8 * width);

          //// then calculate the position within the tile that we're drawing
          //s32 tile_pixel_offset_y = idx2;
          //s32 tile_pixel_offset = tile_top_left_idx + (idx2 * width);


        }

      }
    }

    void fill_texture_data(const map_matrix::MapMatrix& mapMatrix, const BlockSet* blockset, const TileSet& tileset)
    {
      // Go over each cell in the map matrix, and fill in the tiles of the corresponding block of that cell
      rex::ScopedTimer scoped_timer("map generation");

      for (s8 y = 0; y < mapMatrix.height(); ++y)
      {
        for (s8 x = 0; x < mapMatrix.width(); ++x)
        {
          const Block& block = mapMatrix.index_at(x, y);
          fill_block({ x, y }, mapMatrix.width() * Block::width_px(), block, tileset);

          //blockset_index *= num_indices_per_block;
          //rsl::memcpy(block_indices.data(), &blockset[blockset_index], num_indices_per_block);

          // Tiles are stored different in the png than the layout expects
          // layout expects tiles to be stored row by row, which is technically true
          // but you have a lot of padding in between
          // 1 byte is used per pixel
          //for (s8 idx = 0; idx < block_indices.size(); ++idx)
          //{
          //  s32 tile_idx = block_indices[idx];
          //  s32 tx = tile_idx % 16;
          //  s32 ty = tile_idx / 16;
          //  s32 tile_data_start = 8 * ty * tileset_width + (tx * 8);

          //  for (s8 idx2 = 0; idx2 < 8; ++idx2)
          //  {
          //    stbi_uc* tile_row_data = tileset_data + tile_data_start + (idx2 * tileset_width);

          //    // copy the tile row data into the right place in the texture memory
          //    // first calculate the top left coordinate of the block
          //    s32 block_top_left_offset = y * 32 * width + (x * 32);

          //    // then calculate the top left coordinate of the current tile we're processing
          //    s32 tile_offset_x = idx % 4; // tiles per block row
          //    s32 tile_offset_y = idx / 4; // tiles per block column
          //    s32 tile_top_left_idx = block_top_left_offset + (tile_offset_x * 8) + (tile_offset_y * 8 * width);

          //    // then calculate the position within the tile that we're drawing
          //    s32 tile_pixel_offset_y = idx2;
          //    s32 tile_pixel_offset = tile_top_left_idx + (idx2 * width);

          //    REX_ASSERT_X(texture_data.get() + 7 <= texture_data.get() + texture_data.byte_size(), "Would write outside of texture boundary");
          //    REX_ASSERT_X(tile_row_data + 7 <= tileset_data + (tileset_width * tileset_height * num_channels), "Would read outside of texture boundary");

          //    //rsl::memcpy(m_texture_data.get() + tile_pixel_offset, tile_row_data, 8);
          //    for (s8 i = 0; i < 8; ++i)
          //    {
          //      rsl::Rgba& data = texture_data[tile_pixel_offset++];
          //      f32 color = tile_row_data[i];
          //      data.red = color;
          //      data.green = color;
          //      data.blue = color;
          //      data.alpha = 255;
          //    }
          //  }
          //}
        }
      }

      auto ns = scoped_timer.stop();
      REX_INFO(LogGameSession, "Map generation took {} ns", ns);
    }

  private:
    rsl::unique_array<rsl::Rgba> m_texture_data;
  };





  Map load_map(rsl::string_view filepath)
  {
    // Load the map data, this is loading the first bits of information from disk
    // This will determine what we need to load later on
    return MapGenerator{}.generate(filepath);

    //rsl::unique_ptr<MapData> map_object = load_map_data(filepath);

    //// Create the map matrix, which is a matrix of the map, with the padding, holding a block index in each cell
    //map_matrix::MapMatrix map_matrix(map_object.get());

    //// Based on the map matrix, fill in the pixel of a texture, that comes from overlaying the matrix on the texture
    //MapTextureData map_texture_data(map_object.get(), map_matrix);

    //s8 block_width_px = 8 * 4; // 4 * tile width
    //s8 block_height_px = 8 * 4; // 4 * tile height

    //s16 width = map_matrix.width() * block_width_px;
    //s16 height = map_matrix.height() * block_height_px;


    //// Load the tileset into memory
    //s32 tileset_width, tileset_height, num_channels;
    //rex::memory::Blob file_content = rex::vfs::read_file(map->map_header.blockset);
    //rex::json::json blockset_json = rex::json::parse(file_content);
    //rsl::string_view tileset = blockset_json["tileset"];
    //stbi_uc* tileset_data = stbi_load(tileset.data(), &tileset_width, &tileset_height, &num_channels, 0);

    //rex::memory::Blob blockset_data = rex::vfs::read_file(blockset_json["blockset"]);
    //rsl::unique_array<u8> blockset = blockset_data.release_as_array<u8>();

    //// Allocate the map plane data. The pixel data will copied into this, before its sent to the gpu
    //rsl::unique_array<rsl::Rgba> texture_data = rsl::make_unique<rsl::Rgba[]>(width * height);

    //// Go over every block and assign the data of the tile to it
    //const s8 num_indices_per_block = 16;
    //rsl::array<s8, num_indices_per_block> block_indices;
    //for (s8 y = 0; y < map_matrix.height(); ++y)
    //{
    //  for (s8 x = 0; x < map_matrix.width(); ++x)
    //  {
    //    s32 blockset_index = map_matrix.index_at(x, y);
    //    blockset_index *= num_indices_per_block;
    //    rsl::memcpy(block_indices.data(), &blockset[blockset_index], num_indices_per_block);

    //    // Tiles are stored different in the png than the layout expects
    //    // layout expects tiles to be stored row by row, which is technically true
    //    // but you have a lot of padding in between
    //    // 1 byte is used per pixel
    //    for (s8 idx = 0; idx < block_indices.size(); ++idx)
    //    {
    //      s32 tile_idx = block_indices[idx];
    //      s32 tx = tile_idx % 16;
    //      s32 ty = tile_idx / 16;
    //      s32 tile_data_start = 8 * ty * tileset_width + (tx * 8);

    //      for (s8 idx2 = 0; idx2 < 8; ++idx2)
    //      {
    //        stbi_uc* tile_row_data = tileset_data + tile_data_start + (idx2 * tileset_width);

    //        // copy the tile row data into the right place in the texture memory
    //        // first calculate the top left coordinate of the block
    //        s32 block_top_left_offset = y * 32 * width + (x * 32);

    //        // then calculate the top left coordinate of the current tile we're processing
    //        s32 tile_offset_x = idx % 4; // tiles per block row
    //        s32 tile_offset_y = idx / 4; // tiles per block column
    //        s32 tile_top_left_idx = block_top_left_offset + (tile_offset_x * 8) + (tile_offset_y * 8 * width);

    //        // then calculate the position within the tile that we're drawing
    //        s32 tile_pixel_offset_y = idx2;
    //        s32 tile_pixel_offset = tile_top_left_idx + (idx2 * width);

    //        REX_ASSERT_X(texture_data.get() + 7 <= texture_data.get() + texture_data.byte_size(), "Would write outside of texture boundary");
    //        REX_ASSERT_X(tile_row_data + 7 <= tileset_data + (tileset_width * tileset_height * num_channels), "Would read outside of texture boundary");

    //        //rsl::memcpy(m_texture_data.get() + tile_pixel_offset, tile_row_data, 8);
    //        for (s8 i = 0; i < 8; ++i)
    //        {
    //          rsl::Rgba& data = texture_data[tile_pixel_offset++];
    //          f32 color = tile_row_data[i];
    //          data.red = color;
    //          data.green = color;
    //          data.blue = color;
    //          data.alpha = 255;
    //        }
    //      }
    //    }
    //  }
    //}

    //// Pass this data over to create the texture
    //rsl::unique_ptr<rex::gfx::Texture2D> map_texture = rex::gfx::rhi::create_texture2d(width, height, rex::gfx::TextureFormat::Unorm4, texture_data.get());


    //return Map();
  }

  //rsl::unique_ptr<rex::gfx::Texture2D> create_texture_for_map(const MapData* map)
  //{
  //  map_matrix::MapMatrix map_matrix(map);
  //  
  //  s8 block_width_px = 8 * 4; // 4 * tile width
  //  s8 block_height_px = 8 * 4; // 4 * tile height

  //  s16 width = map_matrix.width() * block_width_px;
  //  s16 height = map_matrix.height() * block_height_px;


  //  // Load the tileset into memory
  //  s32 tileset_width, tileset_height, num_channels;
  //  rex::memory::Blob file_content = rex::vfs::read_file(map->map_header.blockset);
  //  rex::json::json blockset_json = rex::json::parse(file_content);
  //  rsl::string_view tileset = blockset_json["tileset"];
  //  stbi_uc* tileset_data = stbi_load(tileset.data(), &tileset_width, &tileset_height, &num_channels, 0);

  //  rex::memory::Blob blockset_data = rex::vfs::read_file(blockset_json["blockset"]);
  //  rsl::unique_array<u8> blockset = blockset_data.release_as_array<u8>();

  //  // Allocate the map plane data. The pixel data will copied into this, before its sent to the gpu
  //  rsl::unique_array<rsl::Rgba> texture_data = rsl::make_unique<rsl::Rgba[]>(width * height);

  //  // Go over every block and assign the data of the tile to it
  //  const s8 num_indices_per_block = 16;
  //  rsl::array<s8, num_indices_per_block> block_indices;
  //  for (s8 y = 0; y < map_matrix.height(); ++y)
  //  {
  //    for (s8 x = 0; x < map_matrix.width(); ++x)
  //    {
  //      s32 blockset_index = map_matrix.index_at(x, y);
  //      blockset_index *= num_indices_per_block;
  //      rsl::memcpy(block_indices.data(), &blockset[blockset_index], num_indices_per_block);

  //      // Tiles are stored different in the png than the layout expects
  //      // layout expects tiles to be stored row by row, which is technically true
  //      // but you have a lot of padding in between
  //      // 1 byte is used per pixel
  //      for (s8 idx = 0; idx < block_indices.size(); ++idx)
  //      {
  //        s32 tile_idx = block_indices[idx];
  //        s32 tx = tile_idx % 16;
  //        s32 ty = tile_idx / 16;
  //        s32 tile_data_start = 8 * ty * tileset_width + (tx * 8);

  //        for (s8 idx2 = 0; idx2 < 8; ++idx2)
  //        {
  //          stbi_uc* tile_row_data = tileset_data + tile_data_start + (idx2 * tileset_width);

  //          // copy the tile row data into the right place in the texture memory
  //          // first calculate the top left coordinate of the block
  //          s32 block_top_left_offset = y * 32 * width + (x * 32);

  //          // then calculate the top left coordinate of the current tile we're processing
  //          s32 tile_offset_x = idx % 4; // tiles per block row
  //          s32 tile_offset_y = idx / 4; // tiles per block column
  //          s32 tile_top_left_idx = block_top_left_offset + (tile_offset_x * 8) + (tile_offset_y * 8 * width);

  //          // then calculate the position within the tile that we're drawing
  //          s32 tile_pixel_offset_y = idx2;
  //          s32 tile_pixel_offset = tile_top_left_idx + (idx2 * width);

  //          REX_ASSERT_X(texture_data.get() + 7 <= texture_data.get() + texture_data.byte_size(), "Would write outside of texture boundary");
  //          REX_ASSERT_X(tile_row_data + 7 <= tileset_data + (tileset_width * tileset_height * num_channels), "Would read outside of texture boundary");

  //          //rsl::memcpy(m_texture_data.get() + tile_pixel_offset, tile_row_data, 8);
  //          for (s8 i = 0; i < 8; ++i)
  //          {
  //            rsl::Rgba& data = texture_data[tile_pixel_offset++];
  //            f32 color = tile_row_data[i];
  //            data.red = color;
  //            data.green = color;
  //            data.blue = color;
  //            data.alpha = 255;
  //          }
  //        }
  //      }
  //    }
  //  }

  //  // Pass this data over to create the texture
  //  return rex::gfx::rhi::create_texture2d(width, height, rex::gfx::TextureFormat::Unorm4, texture_data.get());
  //}

  GameSession::GameSession()
  {
		// We don't want any assets, script, resource, to magically load in another one, all of this needs to be explicitly defined in data, not in code.
		// However, knowing what to load when the game boots up is tricky to specify in data.
		// It's a bit of a chicken and egg problem. To just get it over with, we have a startup save file
		// This acts like any other save file and holds all the data to initialize the game on first startup
		// Any other save file gets loaded on top of this save file, overwriting data where needed

    SaveFile startup_save_file = load_startup_savefile();

    // Load the map the player is currently located in
    Map map = load_map(startup_save_file.current_map);

    //rsl::unique_ptr<rex::gfx::Texture2D> map_texture = create_texture_for_map(m_active_map.get());

    // Convert the map to a map matrix, holding the block index in every block
    //map_matrix::MapMatrix map_matrix(m_active_map.get());

    // Create the map plane out of the map matrix, this is thre graphical representation of the map
    //MapPlane map_plane(m_active_map.get(), map_matrix);


    // Pass the map to the renderer so it can start preparing it
    //renderer::load_map(m_active_map.get());
  }

  SaveFile GameSession::load_startup_savefile() const
  {
    // The startup save file is located at the root directory of the project
    rsl::string startup_save_filepath(rex::cmdline::get_argument("StartupSaveFile").value_or(rex::path::join(rex::vfs::project_root(), "startup_save_file.json")));
    REX_ASSERT_X(rex::file::exists(startup_save_filepath), "startup save filepath does not exist.");
    return SaveFile(startup_save_filepath);
  }


  //rsl::point<s8> square_to_block(rsl::point<s8> square)
  //{
  //  rsl::point<s8> block{};
  //  block.x = square.x / 2;
  //  block.y = square.y / 2;
  //  return block;
  //}

  //void render_map_block(rsl::point<s8> block)
  //{

  //}

  //void render_north_connection_block(rsl::point<s8> coord, MapConnection* connection)
  //{
  //  rsl::point<s8> connection_coord = coord;
  //  connection_coord.y -= 1; // provide an additional offset or square -1 would be mapped on block 0
  //  if (!connection)
  //  {
  //    return render_border_block(connection_coord);
  //  }

  //  rsl::point<s8> offset_block{};
  //  offset_block.x += connection->offset();
  //  offset_block.y += connection->height();
  //  render_connection_block(connection, offset_block);
  //}
  //void render_south_connection_block(rsl::point<s8> coord, MapConnection* connection)
  //{
  //  rsl::point<s8> connection_coord = coord;
  //  if (!connection)
  //  {
  //    return render_border_block(connection_coord);
  //  }

  //  rsl::point<s8> offset_block{};
  //  offset_block.x += connection->offset();
  //  offset_block.y -= m_current_map->height();
  //  render_connection_block(connection, offset_block);
  //}
  //void render_east_connection_block(rsl::point<s8> coord, MapConnection* connection)
  //{
  //  rsl::point<s8> connection_coord = coord;
  //  if (!connection)
  //  {
  //    return render_border_block(connection_coord);
  //  }

  //  rsl::point<s8> offset_block{};
  //  offset_block.x -= m_current_map->width();
  //  offset_block.y += connection->offset();
  //  render_connection_block(connection, offset_block);
  //}
  //void render_west_connection_block(rsl::point<s8> coord, MapConnection* connection)
  //{
  //  rsl::point<s8> connection_coord = coord;
  //  connection_coord.x -= 1; // provide an additional offset or square -1 would be mapped on block 0
  //  if (!connection)
  //  {
  //    return render_border_block(connection_coord);
  //  }

  //  rsl::point<s8> offset_block{};
  //  offset_block.x += connection->width();
  //  offset_block.y += connection->offset();
  //  render_connection_block(connection, offset_block);

  //}

  //GameSession::GameSession()
  //  : m_scene_camera(glm::vec3(0.0f, 5.0f, -20.0f), rsl::deg_angle(45.0f), static_cast<f32>(1280), static_cast<f32>(720), 0.1f, 1000.0f)
  //  , m_scene_viewport_width(1280)
  //  , m_scene_viewport_height(720)
  //{
  //  rex::vfs::mount(rex::MountingPoint::Maps, rex::path::join(rex::vfs::project_root(), "dev", "maps"));
  //  rex::vfs::mount(rex::MountingPoint::Tilesets, rex::path::join(rex::vfs::project_root(), "retail", "tilesets"));
  //  m_scene = rsl::make_unique<rex::gfx::Scene>();
  //  m_scene_renderer = rex::gfx::add_renderer<rex::gfx::SceneRenderer>();
  //  m_light_direction.y = -1.0f;
  //  m_light_direction.z = 2.0f;

  //  initialize_common_resources();
  //  initialise_world();
  //}

  //void GameSession::update()
  //{
  //  update_background_map();




  //  rex::gfx::SceneData scene_data{};
  //  scene_data.scene = m_scene.get();
  //  scene_data.camera = &m_scene_camera;
  //  scene_data.viewport_width = m_scene_viewport_width;
  //  scene_data.viewport_height = m_scene_viewport_height;
  //  scene_data.light_direction = m_light_direction;
  //  m_scene_renderer->update_scene_data(scene_data);
  //}

  //void GameSession::initialize_common_resources()
  //{
  //  // Load the sampler used by chunks
  //  // ..
  //}

  //void GameSession::initialise_world()
  //{
  //  // We don't want any assets, script, resource, to magically load in another one, all of this needs to be explicitly defined in data, not in code.
  //  // However, knowing what to load when the game boots up is tricky to specify in data.
  //  // It's a bit of a chicken and egg problem. To just get it over with, we have a startup save file
  //  // This acts like any other save file and holds all the data to initialize the game on first startup
  //  // Any other save file gets loaded on top of this save file, overwriting data where needed

  //  // The startup save file is located at the root directory of the project
  //  rsl::string startup_save_filepath(rex::cmdline::get_argument("StartupSaveFile").value_or(rex::path::join(rex::vfs::project_root(), "startup_save_file.json")));
  //  SaveFile startup_save_file(startup_save_filepath);

  //  // After the startup info has been loaded, load the current map the player is in
  //  m_current_map = load_map(startup_save_file.current_map);
  //  REX_ASSERT_X(m_current_map != nullptr, "Failed to load map");

  //  // Once the current map is loaded, load in the chunk the player is in
  //  //m_current_map->load_chunk(startup_save_file.current_chunk);
  //}

  //void GameSession::update_background_map()
  //{
  //  // The goal of this function is to have everything prepared for the renderer to render the background
  //  // The background consists of tiles, each 8 by 8 pixels
  //  // These tiles are indexed from blocks and the blocks are indexed in the map itself

  //  // we're rendering 5x4.5 blocks, with the player always at the center of the screen
  //  // each map has a border block which will get tiled if no connection to a neighbouring map is found
  //  // If a connection is found however, we render that connection's blocks instead

  //  // The player's position is always relative to the top left of the screen.
  //  // Squares are 2x2 tiles. The player is always in square (4, 4)
  //  // This means the player's block is always (2,2)
  //  rsl::point<s8> top_left_offset_from_player{ -2, -2 };
  //  const s8 blocks_to_render_on_x = 6;
  //  const s8 blocks_to_render_on_y = 5;
  //  const s8 num_blocks_to_render = blocks_to_render_on_x * blocks_to_render_on_y;
  //  rsl::point<s8> current_render_coord = m_player_block_pos - top_left_offset_from_player;
  //  rsl::array<s8, num_blocks_to_render> blocks_indices_to_render;
  //  for (s32 row = 0; row < blocks_to_render_on_y; ++row)
  //  {
  //    current_render_coord.x = 0;
  //    current_render_coord.y++;
  //    for (s32 column = 0; column < blocks_to_render_on_x; ++column)
  //    {
  //      s8 idx_into_block_arr = row * blocks_to_render_on_x + column;
  //      s8 block_idx = m_current_map->block_idx_at(current_render_coord);
  //      blocks_indices_to_render[idx_into_block_arr] = block_idx

  //      //// if it falls within the map dimension, lookup the block and render it
  //      //if (block_within_map_dimension(current_render_coord))
  //      //{
  //      //  blocks_indices_to_render = m_current_map->block_at(current_render_coord);
  //      //  render_map_block(current_render_coord);
  //      //}
  //      //// If not render either the block of the connected map or the border block
  //      //else
  //      //{
  //      //  MapConnection* target_connection = nullptr;
  //      //  if (current_render_coord.x < 0)
  //      //  {
  //      //    target_connection = m_current_map->west_connection();
  //      //    render_west_connection_block(current_render_coord, target_connection);
  //      //  }
  //      //  else if (current_render_coord.x >= target_connection->width())
  //      //  {
  //      //    target_connection = m_current_map->east_connection();
  //      //    render_east_connection_block(current_render_coord, target_connection);
  //      //  }
  //      //  if (current_render_coord.y < 0)
  //      //  {
  //      //    target_connection = m_current_map->north_connection();
  //      //    render_north_connection_block(current_render_coord, target_connection);
  //      //  }
  //      //  else if (current_render_coord.y >= m_current_map->height())
  //      //  {
  //      //    target_connection = m_current_map->south_connection();
  //      //    render_south_connection_block(current_render_coord, target_connection);
  //      //  }
  //      //}

  //      current_render_coord.x++;
  //    }
  //  }

  //  m_renderer->update_block_indices(blocks_indices_to_render);

  //  // If the player is no longer located in the current map
  //  // update the state so the player is now 
  //  if (!not_in_map(player_pos, m_current_map))
  //  {
  //    enter_map(m_current_map);
  //  }

  //  // Get the player location and calculate which blocks need to be drawn for this
  //  // Based on the player location, we need to render its location - (5,4)
  //  // If this is in another map, we need to render it as well

  //  s8 overlapping_blocks = 3;
  //  glm::vec2 player_pos;
  //  if (player_pos.x < overlapping_blocks)
  //  {
  //    // render west map as well
  //  }
  //  else if (player_pos.x > m_current_map->width() - overlapping_blocks)
  //  {
  //    // render east map as well
  //  }
  //  if (player_pos.y < overlapping_blocks)
  //  {
  //    // render north map as well
  //  }
  //  else if (player_pos.y > m_current_map->height() - overlapping_blocks)
  //  {
  //    // render south map as well
  //  }

  //}

  //rsl::unique_ptr<Map> GameSession::load_map(rsl::string_view filepath)
  //{
  //  // Always check if the map exists, you never know, it might not..
  //  if (!rex::vfs::is_file(filepath))
  //  {
  //    REX_ERROR(LogGameSession, "Failed to load map. Filepath doesn't exist. filepath: {}", filepath);
  //    return nullptr;
  //  }

  //  // Load the json of the map into memory and parse it
  //  // Check if it's a valid json as again, you never know, it might not be
  //  rex::json::json map_json = rex::json::read_from_file(filepath);
  //  if (map_json.is_discarded())
  //  {
  //    REX_ERROR(LogGameSession, "Failed to load map. JSON parsing error. filepath: {}", filepath);
  //    return nullptr;
  //  }

  //  // If all of the above is correct, we should have a proper json format in memory now
  //  // Pass this to the constructor of the map and let it take care of the next steps
  //  return rsl::make_unique<Map>(m_scene.get(), map_json);
  //}
}
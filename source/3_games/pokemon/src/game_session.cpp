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
#include "rex_engine/images/image_loader.h"
#include "rex_engine/memory/array_view.h"

#include "rex_engine/gfx/resources/vertex_buffer.h"
#include "rex_engine/gfx/resources/index_buffer.h"

#include "pokemon/block.h"
#include "pokemon/blockset.h"
#include "pokemon/tileset.h"

#include "rex_engine/profiling/instrumentor.h"

namespace pokemon
{
  DEFINE_LOG_CATEGORY(LogGameSession);

  // This holds the information about the map, width, height, blockset to use, ...
  rsl::unordered_map<rsl::string_view, rsl::unique_ptr<MapData>> g_maps;

  // This holds the map blcoks themselves, as in the block indices themselves
  rsl::unordered_map<rsl::string_view, rsl::unique_array<u8>> g_map_blocks;

  struct MapRenderData
  {
    rsl::shared_ptr<BlockSet> blockset;
    rsl::unique_ptr<rex::gfx::Texture2D> tileset;
  };

  rsl::unordered_map<rsl::string_view, MapRenderData> g_map_render_data;





  MapData* find_map(rsl::string_view mapPath);
  MapData* find_map_without_connections(rsl::string_view mapPath);

  MapHeader load_map_header(const rex::json::json& jsonBlob);
  MapHeader load_map_header(rsl::string_view mapPath);

  void load_map_data(MapData* mapObject, const rex::json::json& jsonBlob);
  rsl::unique_ptr<MapData> load_map_data(rsl::string_view mapPath);
  rsl::unique_ptr<MapData> load_map_header_only(rsl::string_view mapPath);

  const rsl::unique_array<u8>& find_map_blocks(const MapHeader& mapHeader)
  {
    if (!g_map_blocks.contains(mapHeader.map_blocks_filepath))
    {
      rex::memory::Blob file_content = rex::vfs::read_file(mapHeader.map_blocks_filepath);
      return g_map_blocks.emplace(mapHeader.map_blocks_filepath, rsl::unique_array<u8>(file_content.release_as_array<u8>())).inserted_element->value;
    }

    return g_map_blocks.at(mapHeader.map_blocks_filepath);
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
    map_header.map_blocks_filepath = jsonBlob["map_blocks"];
    map_header.map_render_data = jsonBlob["blockset"];
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

    s32 coord_to_index(s32 x, s32 y, s32 width)
    {
      return y * width + x;
    }

    rsl::pointi8 index_to_coord(s32 idx, s32 width, s32 height)
    {
      rsl::pointi8 coord{};

      coord.x = idx % width;
      coord.y = idx / height;

      return coord;
    }

    // The map matrix is a matrix of the map, including the padding blocks
    // Each cell in the matrix holds a block index.
    // All blocks in the map matrix are expected to use the same blockset
    class MapMatrix
    {
    public:
      MapMatrix(const MapData* mapObject, const BlockSet* blockset)
      {
        m_total_width = mapObject->map_header.width + (2 * g_padding);
        m_total_height = mapObject->map_header.height + (2 * g_padding);

        m_block_indices = rsl::make_unique<u8[]>(m_total_width * m_total_height);
        init_blocks(mapObject->map_header.border_block_idx);
        
        fill_connections(mapObject);
        fill_inner_map(mapObject);

        fill_tile_cache(blockset);
      }

      s32 width_in_blocks() const
      {
        return m_total_width;
      }
      s32 height_in_blocks() const
      {
        return m_total_height;
      }
      s32 major_dimension_in_blocks() const
      {
        return width_in_blocks();
      }
      s32 width_in_tiles() const
      {
        return width_in_blocks() * Block::num_tiles_per_row();
      }
      s32 height_in_tiles() const
      {
        return height_in_blocks() * Block::num_tiles_per_column();
      }
      s32 major_dimension_in_tiles() const
      {
        return width_in_blocks() * Block::num_tiles_per_row();
      }

      s32 index_at(s32 idx) const
      {
        return m_block_indices[idx];
      }
      s32 index_at(s32 x, s32 y) const
      {
        s32 idx = coord_to_index(x, y, m_total_width);
        return m_block_indices[idx];
      }

    private:
      void init_blocks(s8 borderBlockIdx)
      {
        rsl::fill_n(m_block_indices.begin(), m_block_indices.count(), borderBlockIdx);
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
          const rsl::unique_array<u8>& conn_map_blocks = find_map_blocks(conn.map->map_header);

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
      void fill_inner_map(const MapData* mapObject)
      {
        s32 height = mapObject->map_header.height;
        s32 width = mapObject->map_header.width;

        const rsl::unique_array<u8>& map_blocks = find_map_blocks(mapObject->map_header);
        for (s8 y = 0; y < height; ++y)
        {
          for (s8 x = 0; x < width; ++x)
          {
            s32 index = y * width + x;
            s16 map_matrix_index = ((y + g_padding) * m_total_width) + (x + g_padding);
            m_block_indices[map_matrix_index] = map_blocks[index++];
          }
        }
      }

      void fill_tile_cache(const BlockSet* blockset)
      {
        m_tile_cache = rsl::make_unique<u8[]>(width_in_tiles() * height_in_tiles());
        for (s32 y = 0; y < height_in_blocks(); ++y)
        {
          for (s32 x = 0; x < width_in_blocks(); ++x)
          {
            s32 index_in_map_matrix = map_matrix::coord_to_index(x, y, major_dimension_in_blocks());
            s32 block_idx = index_at(index_in_map_matrix);

            const Block& block = blockset->block(block_idx);

            for (s32 ty = 0; ty < Block::num_tiles_per_row(); ++ty)
            {
              for (s32 tx = 0; tx < Block::num_tiles_per_column(); ++tx)
              {
                s8 index_in_block = map_matrix::coord_to_index(tx, ty, Block::major_dimension());
                u8 tile_idx = block.index_at(index_in_block);

                u8 ttx = x * Block::num_tiles_per_column() + tx;
                u8 tty = y * Block::num_tiles_per_row() + ty;

                s32 idx = map_matrix::coord_to_index(ttx, tty, width_in_blocks() * Block::num_tiles_per_column());
                m_tile_cache[idx] = tile_idx;
              }
            }
          }
        }
      }

    private:
      rsl::unique_array<u8> m_block_indices;
      rsl::unique_array<u8> m_tile_cache;
      s8 m_total_width;
      s8 m_total_height;
    };
  }

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

  struct TileRendererDesc
  {
    s32 width_in_tiles;
    s32 height_in_tiles;
    rex::gfx::Texture2D* tileset_texture;
  };
  struct TileVertex
  {
    rsl::point<f32> pos;
    rsl::point<f32> uv;
  };
  struct TileWVP
  {
    f32 wvp[4][4];
  };


  class TileRenderer
  {
  public:
    TileRenderer(const TileRendererDesc& desc)
      : m_width(desc.width_in_tiles)
      , m_height(desc.height_in_tiles)
      , m_tileset_texture(desc.tileset_texture)
    {
      m_tiles_vb_cpu = rsl::make_unique<TileVertex[]>(expected_num_vertices());
      m_tiles_ib_cpu = rsl::make_unique<u16[]>(expected_num_indices());

      m_tiles_vb_gpu = rex::gfx::rhi::create_vertex_buffer(expected_num_vertices(), sizeof(TileVertex));
      m_tiles_ib_gpu = rex::gfx::rhi::create_index_buffer(expected_num_indices(), rex::gfx::IndexBufferFormat::Uint16);
    }

    void update_tileset_texture(const rex::gfx::Texture2D* texture);
    void update_dimension(s32 width, s32 height);

    void render(const rex::ArrayView<TileVertex>& vertices, const rex::ArrayView<TileWVP>& tileWvps)
    {
      REX_ASSERT_X(vertices.count() == expected_num_vertices(), "Invalid number of vertices, got: {}, expected: {}", vertices.count(), expected_num_vertices());
      REX_ASSERT_X(tileWvps.count() == expected_num_wvps(), "Invalid number of wvps, got: {}, expected: {}", tileWvps.count(), expected_num_wvps());

      // Update the vertex buffer with the vertices
      // Update the wvps buffer with the wvps
      auto copy_ctx = rex::gfx::new_copy_ctx();

      copy_ctx->update_buffer(m_tiles_vb_gpu.get(), vertices.data(), vertices.count() * sizeof(TileVertex));
      //copy_ctx->update_buffer(m_tiles_ib_gpu.get(), )

    }

  private:
    s32 expected_num_vertices() const
    {
      constexpr s32 num_vertices_per_tile = 4;
      return m_width * m_height * num_vertices_per_tile;
    }
    s32 expected_num_tiles() const
    {
      return m_width * m_height;
    }
    s32 expected_num_wvps() const
    {
      return expected_num_tiles();
    }
    s32 expected_num_indices() const
    {
      constexpr s32 num_indices_per_tile = 6;
      return expected_num_tiles() * num_indices_per_tile;
    }

  private:
    s32 m_width; // width in tiles
    s32 m_height; // height in tiles
    rex::gfx::Texture2D* m_tileset_texture;

    // Create tiles vertex buffer and wvp buffer in the renderer
    // these get updated every frame
    rsl::unique_ptr<rex::gfx::VertexBuffer> m_tiles_vb_gpu;
    rsl::unique_ptr<rex::gfx::IndexBuffer> m_tiles_ib_gpu;

    rsl::unique_array<TileVertex> m_tiles_vb_cpu;
    rsl::unique_array<u16> m_tiles_ib_cpu;
  };

  rsl::unique_ptr<rex::gfx::Texture2D> create_tileset_texture(rsl::string_view filepath)
  {
    rex::ImageLoadResult tileset_img_load_res = rex::load_image(filepath);

    // A tileset only holds 1 channel, we have to convert it to 4 channels as that's what the GPU expects
    rsl::unique_array<rsl::Rgba> tileset_rgba = rsl::make_unique<rsl::Rgba[]>(tileset_img_load_res.width * tileset_img_load_res.height * sizeof(rsl::Rgba));
    for (s32 color_idx = 0; color_idx < tileset_img_load_res.width * tileset_img_load_res.height; ++color_idx)
    {
      u8 color = tileset_img_load_res.data[color_idx];
      rsl::Rgba& rgba = tileset_rgba[color_idx];
      rgba.red = color;
      rgba.green = color;
      rgba.blue = color;
      rgba.alpha = 255;
    }

    rsl::unique_ptr<rex::gfx::Texture2D> tileset_texture = rex::gfx::rhi::create_texture2d(tileset_img_load_res.width, tileset_img_load_res.height, rex::gfx::TextureFormat::Unorm4, tileset_rgba.get());
    return tileset_texture;
  }

  MapRenderData load_map_render_data(rsl::string_view filepath)
  {
    rex::json::json blockset_json = rex::json::read_from_file(filepath);
    rsl::string_view tileset_filepath = blockset_json["tileset"];

    MapRenderData res{};
    res.blockset = load_blockset(blockset_json["blockset"]);
    res.tileset = create_tileset_texture(tileset_filepath);

    return res;
  }

  class MapGenerator
  {
  public:
    Map generate(const MapData* mapData)
    {
      // We have a new goal here now to reduce memory usage during runtime
      // Create an vertex buffer holding the vertices for each tile
      // A vertex for a single tile consist of the following
      // X, Y -> 8 bytes
      // U, V -> 8 bytes
      // total: 16 bytes
      // The WVP matrix for each tile will be sent in a buffer of its own so we have 1 WVP for each tile instead of 1 per vertex
      // This will use per instance data.
      // The original game boy screen width is 160 x 144 pixels, meaning we're drawing 20 x 18 tiles
      // This results in a total of 360 tiles
      // Each tile has 4 vertices, each vertex is 16 bytes
      // 4 * 16 * 360 = 23040 bytes, this is the size of the vertex buffer
      // we also need to upload 1 world view projection per tile
      // each world view projection matrix is a 4 x 4 matrix, each matrix being 16 bytes
      // 360 x 16 = 5760
      // This means we can render the viewport using only 28 800 bytes, at least for the map itself, excluding the player sprite and NPCs

      // Load the map render data, which is the tileset and blockset into memory
      MapRenderData map_render_data = load_map_render_data(mapData->map_header.map_render_data_filepath);

      // Create the map matrix, which is a matrix of the map, with the padding, holding a block index in each cell
      map_matrix::MapMatrix map_matrix(mapData, map_render_data.blockset.get());

      s32 num_tiles = map_matrix.width_in_tiles() * map_matrix.height_in_tiles();
      s32 num_vertices_per_tile = 4;
      rsl::unique_array<TileVertex> tiles_vb = rsl::make_unique<TileVertex[]>(num_vertices_per_tile * num_tiles);
      rsl::unique_array<TileWVP> tile_wvps = rsl::make_unique<TileWVP[]>(num_tiles);

      TileRendererDesc tile_renderer_desc{};
      tile_renderer_desc.width_in_tiles = map_matrix.width_in_tiles();
      tile_renderer_desc.height_in_tiles = map_matrix.height_in_tiles();
      tile_renderer_desc.tileset_texture = map_render_data.tileset.get();

      TileRenderer tile_renderer(tile_renderer_desc);


      // Tile rendering pipeline
      // 1. Calculate top left tile based on player position
      rsl::pointi8 player_pos = { 5,5 };
      // 2. Use map matrix to get the tile matrix
      const rex::ArrayView<u8>& tile_matrix = map_matrix.update_tile_matrix(player_pos);
      // 3. Convert the tile matrix into a list of vertices and wvps
      TileMatrixRenderData tile_matrix_render_data = convert_to_render_data(tile_matrix);
      // 4. Pass the vertices and wvps to the rnederer
      tile_renderer.render(tile_matrix);




      tile_renderer.render();

      // Calculate the dimensions of the texture
      s16 width = map_matrix.width_in_blocks() * Block::width_px();
      s16 height = map_matrix.height_in_blocks() * Block::height_px();

      // Allocate the memory to fill in the texture data
      m_texture_data = rsl::make_unique<rsl::Rgba[]>(width * height);
      rsl::unique_ptr<rex::gfx::Texture2D> map_texture = rex::gfx::rhi::create_texture2d(width, height, rex::gfx::TextureFormat::Unorm4, m_texture_data.get());

      return Map(rsl::move(map_texture));
    }

  private:
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

          for (s8 i = 0; i < 8; ++i)
          {
            rsl::Rgba& data = tile_row_data_dst[i];
            u8 color = tile_row_data_src[i];
            data.red = color;
            data.green = color;
            data.blue = color;
            data.alpha = 255;
          }
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
          u8 block_idx = mapMatrix.index_at(x, y);
          fill_block({ x, y }, mapMatrix.width() * Block::width_px(), blockset->block(block_idx), tileset);
        }
      }

      auto ns = scoped_timer.stop();
      REX_INFO(LogGameSession, "Map generation took {} ns", ns.count());
    }

  private:
    rsl::unique_array<rsl::Rgba> m_texture_data;
  };

  Map load_map(const MapData* mapData)
  {
    // Load the map data, this is loading the first bits of information from disk
    // This will determine what we need to load later on
    return MapGenerator{}.generate(mapData);
  }

  GameSession::GameSession()
  {
		// We don't want any assets, script, resource, to magically load in another one, all of this needs to be explicitly defined in data, not in code.
		// However, knowing what to load when the game boots up is tricky to specify in data.
		// It's a bit of a chicken and egg problem. To just get it over with, we have a startup save file
		// This acts like any other save file and holds all the data to initialize the game on first startup
		// Any other save file gets loaded on top of this save file, overwriting data where needed

    SaveFile startup_save_file = load_startup_savefile();

    init_map(startup_save_file);

    // Load the map the player is currently located in
    //Map map = load_map(startup_save_file.current_map);

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

  void GameSession::init_map(const SaveFile& startupSaveFile)
  {
    // Load the map first, good to check if it exists
    MapData* map_data = find_map(startupSaveFile.current_map);

    // Given the above data, generate the map
    // This is generating the data required to render the map
    MapGenerator map_generator;
    map_generator.generate(map_data);

  }
}
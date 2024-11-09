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

namespace pokemon
{
  DEFINE_LOG_CATEGORY(LogGameSession);

  rsl::unordered_map<rsl::string_view, rsl::unique_ptr<MapObject>> g_maps;

  MapObject* find_map(rsl::string_view mapPath);
  MapObject* find_map_without_connections(rsl::string_view mapPath);

  MapHeader load_map_header(const rex::json::json& jsonBlob);
  MapHeader load_map_header(rsl::string_view mapPath);

  void load_map(MapObject* mapObject, const rex::json::json& jsonBlob);
  rsl::unique_ptr<MapObject> load_map(rsl::string_view mapPath);
  rsl::unique_ptr<MapObject> load_map_header_only(rsl::string_view mapPath);




  MapObject* find_map(rsl::string_view mapPath)
  {
    if (!g_maps.contains(mapPath))
    {
      rsl::unique_ptr<MapObject> map = load_map(mapPath);
      
      // If the map wasn't fully loaded (eg. it was loaded as a connection)
      // We need to load it fully now
      if (!map->fully_loaded)
      {
        rex::memory::Blob file_content = rex::vfs::read_file(mapPath);
        rex::json::json json_blob = rex::json::parse(file_content);
        load_map(map.get(), json_blob);

				map->fully_loaded = true;
      }
      g_maps.emplace(mapPath, rsl::move(map));
    }

    return g_maps.at(mapPath).get();
  }
  MapObject* find_map_without_connections(rsl::string_view mapPath)
  {
    if (!g_maps.contains(mapPath))
    {
      rsl::unique_ptr<MapObject> map_object = load_map_header_only(mapPath);
      map_object->fully_loaded = false;
      g_maps.emplace(mapPath, rsl::move(map_object));
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

  void load_map(MapObject* mapObject, const rex::json::json& jsonBlob)
  {
    // Connections
    mapObject->connections.reserve(jsonBlob["connections"].size());
    for (const auto& connection_json : jsonBlob["connections"])
    {
      MapConnection& connection = mapObject->connections.emplace_back();
      connection.direction = rsl::enum_refl::enum_cast<Direction>(connection_json["direction"].get<rsl::string_view>()).value();
      connection.map = find_map_without_connections(connection_json["map"]);
      connection.offset = connection_json["offset"]; // is in squares (2x2 tiles)
    }

    // Wrap events
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

  rsl::unique_ptr<MapObject> load_map(rsl::string_view mapPath)
  {
    rex::memory::Blob file_content = rex::vfs::read_file(mapPath);
    rex::json::json json_blob = rex::json::parse(file_content);

    rsl::unique_ptr<MapObject> map_object = rsl::make_unique<MapObject>();
    map_object->map_header = load_map_header(json_blob);

    load_map(map_object.get(), json_blob);

    return map_object;
  }
  rsl::unique_ptr<MapObject> load_map_header_only(rsl::string_view mapPath)
  {
    rex::memory::Blob file_content = rex::vfs::read_file(mapPath);
    rex::json::json json_blob = rex::json::parse(file_content);

    REX_ASSERT_X(!json_blob.is_discarded(), "Invalid map json file. {}", rex::quoted(mapPath))
    rsl::unique_ptr<MapObject> map_object = rsl::make_unique<MapObject>();
    map_object->map_header = load_map_header(json_blob);

    return map_object;
  }

  GameSession::GameSession()
  {
		// We don't want any assets, script, resource, to magically load in another one, all of this needs to be explicitly defined in data, not in code.
		// However, knowing what to load when the game boots up is tricky to specify in data.
		// It's a bit of a chicken and egg problem. To just get it over with, we have a startup save file
		// This acts like any other save file and holds all the data to initialize the game on first startup
		// Any other save file gets loaded on top of this save file, overwriting data where needed

    SaveFile startup_save_file = load_startup_savefile();

    // Load the map the player is currently located in
    m_active_map = load_map(startup_save_file.current_map);

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
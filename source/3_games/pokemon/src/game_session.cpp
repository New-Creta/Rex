#include "pokemon/game_session.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_engine/text_processing/json.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"

#include "pokemon/save_file.h"
#include "rex_std/bonus/math.h"

namespace pokemon
{
  DEFINE_LOG_CATEGORY(LogGameSession);

  void render_north_connection_block(Coordinate coord)
  {
    Coordinate connection_coord = coord;
    connection_coord.y -= 1; // provide an additional offset or square -1 would be mapped on block 0
    if (!connection)
    {
      return render_border_block(connection_coord);
    }

    Coordinate offset_block{};
    offset_block.x += connection->offset();
    offset_block.y += connection->height();
    render_connection_block(connection, offset_block);
  }
  void render_south_connection_block(Coordinate coord)
  {
    Coordinate connection_coord = coord;
    if (!connection)
    {
      return render_border_block(connection_coord);
    }

    Coordinate offset_block{};
    offset_block.x += connection->offset();
    offset_block.y -= m_current_map->height();
    render_connection_block(connection, offset_block);
  }
  void render_east_connection_block(Coordinate coord)
  {
    Coordinate connection_coord = coord;
    if (!connection)
    {
      return render_border_block(connection_coord);
    }

    Coordinate offset_block{};
    offset_block.x -= m_current_map->width();
    offset_block.y += connection->offset();
    render_connection_block(connection, offset_block);
  }
  void render_west_connection_block(Coordinate coord, ChunkConnection* connection)
  {
    Coordinate connection_coord = coord;
    connection_coord.x -= 1; // provide an additional offset or square -1 would be mapped on block 0
    if (!connection)
    {
      return render_border_block(connection_coord);
    }

    Coordinate offset_block{};
    offset_block.x += connection->width();
    offset_block.y += connection->offset();
    render_connection_block(connection, offset_block);

  }

  GameSession::GameSession()
    : m_scene_camera(glm::vec3(0.0f, 5.0f, -20.0f), rsl::deg_angle(45.0f), static_cast<f32>(1280), static_cast<f32>(720), 0.1f, 1000.0f)
    , m_scene_viewport_width(1280)
    , m_scene_viewport_height(720)
  {
    rex::vfs::mount(rex::MountingPoint::Maps, rex::path::join(rex::vfs::project_root(), "dev", "maps"));
    rex::vfs::mount(rex::MountingPoint::Tilesets, rex::path::join(rex::vfs::project_root(), "retail", "tilesets"));
    m_scene = rsl::make_unique<rex::gfx::Scene>();
    m_scene_renderer = rex::gfx::add_renderer<rex::gfx::SceneRenderer>();
    m_light_direction.y = -1.0f;
    m_light_direction.z = 2.0f;

    initialize_common_resources();
    initialise_world();
  }

  void GameSession::update()
  {
    update_background_map();




    rex::gfx::SceneData scene_data{};
    scene_data.scene = m_scene.get();
    scene_data.camera = &m_scene_camera;
    scene_data.viewport_width = m_scene_viewport_width;
    scene_data.viewport_height = m_scene_viewport_height;
    scene_data.light_direction = m_light_direction;
    m_scene_renderer->update_scene_data(scene_data);
  }

  void GameSession::initialize_common_resources()
  {
    // Load the sampler used by chunks
    // ..
  }

  void GameSession::initialise_world()
  {
    // We don't want any assets, script, resource, to magically load in another one, all of this needs to be explicitly defined in data, not in code.
    // However, knowing what to load when the game boots up is tricky to specify in data.
    // It's a bit of a chicken and egg problem. To just get it over with, we have a startup save file
    // This acts like any other save file and holds all the data to initialize the game on first startup
    // Any other save file gets loaded on top of this save file, overwriting data where needed

    // The startup save file is located at the root directory of the project
    rsl::string startup_save_filepath(rex::cmdline::get_argument("StartupSaveFile").value_or(rex::path::join(rex::vfs::project_root(), "startup_save_file.json")));
    SaveFile startup_save_file(startup_save_filepath);

    // After the startup info has been loaded, load the current map the player is in
    m_current_map = load_map(startup_save_file.current_map);
    REX_ASSERT_X(m_current_map != nullptr, "Failed to load map");

    // Once the current map is loaded, load in the chunk the player is in
    //m_current_map->load_chunk(startup_save_file.current_chunk);
  }

  void GameSession::update_background_map()
  {
    // The goal of this function is to have everything prepared for the renderer to render the background
    // The background consists of tiles, each 8 by 8 pixels
    // These tiles are indexed from blocks and the blocks are indexed in the map itself

    // we're rendering 5x4.5 blocks, with the player always at the center of the screen
    // each map has a border block which will get tiled if no connection to a neighbouring map is found
    // If a connection is found however, we render that connection's blocks instead

    // The player's position is always relative to the top left of the screen.
    // Squares are 2x2 tiles. The player is always in square (4, 4)
    // This means the player's block is always (2,2)
    s32 squares_to_render_on_x = 10;
    s32 squares_to_render_on_y = 9;
    Coordinate player_block_pos{}; // in squares
    Coordinate top_left_coord{}; // in squares
    top_left_coord.x = player_block_pos.x - squares_to_render_on_x / 2;
    top_left_coord.y = player_block_pos.y - squares_to_render_on_y / 2;
    
    for (s32 row = 0; row < m_current_map->height(); row += 2)
    {
      for (s32 column = 0; column < m_current_map->width(); column += 2)
      {
        // The block coordinate we have to render
        Coordinate current_square{};
        current_square.x = top_left_coord.x + column;
        current_square.y = top_left_coord.y + row;

        // if it falls within the map dimension, lookup the block and render it
        Coordinate current_block{};
        if (block_within_map_dimension(current_square))
        {
          current_block.x = current_square.x / 2;
          current_block.y = current_square.y / 2;
          render_map_block(current_block);
          continue;
        }
        // If not render either the block of the connected map or the border block
        else
        {
          auto render_connection_block_or_border_block = [&](ChunkConnection* connection, Coordinate offset)
          {
            if (!connection)
            {
              render_border_block(current_square);
              return;
            }
            
            // look up the block ID of "offset" coordinate in the connection map
            // and render that one at the location of "current_block"
          };

          ChunkConnection* target_connection = nullptr;
          if (current_square.x < 0)
          {
            render_west_connection_block(current_square);
          }
          else if (current_square.x >= target_connection->width())
          {
            render_east_connection_block(current_square);
          }
          if (current_square.y < 0)
          {
            render_north_connection_block();
          }
          else if (current_square.y >= m_current_map->height())
          {
            render_south_connection_block();
          }
        }
      }
    }

    // If the player is no longer located in the current map
    // update the state so the player is now 
    if (!not_in_map(player_pos, m_current_map))
    {
      enter_map(m_current_map);
    }

    // Get the player location and calculate which blocks need to be drawn for this
    // Based on the player location, we need to render its location - (5,4)
    // If this is in another map, we need to render it as well

    s8 overlapping_blocks = 3;
    glm::vec2 player_pos;
    if (player_pos.x < overlapping_blocks)
    {
      // render west map as well
    }
    else if (player_pos.x > m_current_map->width() - overlapping_blocks)
    {
      // render east map as well
    }
    if (player_pos.y < overlapping_blocks)
    {
      // render north map as well
    }
    else if (player_pos.y > m_current_map->height() - overlapping_blocks)
    {
      // render south map as well
    }

  }

  rsl::unique_ptr<Map> GameSession::load_map(rsl::string_view filepath)
  {
    if (!rex::vfs::is_file(filepath))
    {
      REX_ERROR(LogGameSession, "Failed to load map. Filepath doesn't exist. filepath: {}", filepath);
      return nullptr;
    }

    rex::json::json map_json = rex::json::read_from_file(filepath);
    if (map_json.is_discarded())
    {
      REX_ERROR(LogGameSession, "Failed to load map. JSON parsing error. filepath: {}", filepath);
      return nullptr;
    }

    return rsl::make_unique<Map>(m_scene.get(), map_json);
  }
}
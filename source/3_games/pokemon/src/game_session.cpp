#include "pokemon/game_session.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_engine/text_processing/ini_processor.h"
#include "rex_engine/text_processing/text_processing.h"
#include "rex_engine/text_processing/json.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"

namespace pokemon
{
  DEFINE_LOG_CATEGORY(LogGameSession);

  struct SaveFile
  {
  public:
    SaveFile(rsl::string_view filepath)
    {
      current_map.assign("kanto");
      current_level.assign("pallet_town");
    }

  public:
    rsl::string current_map;
    rsl::string current_level;
  };

  GameSession::GameSession()
    : m_scene_camera(glm::vec3(0.0f, 5.0f, -20.0f), rsl::DegAngle(45.0f), static_cast<f32>(1280), static_cast<f32>(720), 0.1f, 1000.0f)
    , m_scene_viewport_width(1280)
    , m_scene_viewport_height(720)
  {
    rex::vfs::mount(rex::MountingPoint::Maps, rex::path::join(rex::vfs::project_root(), "maps"));
    m_scene = rsl::make_unique<rex::gfx::Scene>();
    m_scene_renderer = rex::gfx::add_renderer<rex::gfx::SceneRenderer>();
    m_light_direction.y = -1.0f;
    m_light_direction.z = 2.0f;

    initialise_world();
  }

  void GameSession::update()
  {
    rex::gfx::SceneData scene_data{};
    scene_data.scene = m_scene.get();
    scene_data.camera = &m_scene_camera;
    scene_data.viewport_width = m_scene_viewport_width;
    scene_data.viewport_height = m_scene_viewport_height;
    scene_data.light_direction = m_light_direction;
    m_scene_renderer->update_scene_data(scene_data);
  }

  void GameSession::initialise_world()
  {
    // We don't want any assets, script, resource, to magically load in another one, all of this needs to be explicitly defined in data, not in code.
    // However, knowing what to load when the game boots up is tricky to specify in data.
    // It's a bit of a chicken and egg problem. To just get it over with, we have a startup save file
    // This acts like any other save file and holds all the data to initialize the game on first startup
    // Any other save file gets loaded on top of this save file, overwriting data where needed

    // The startup save file is located at the root directory of the project
    rsl::string startup_save_filepath = rex::path::join(rex::vfs::project_root(), "startup_save_file.json");
    SaveFile startup_save_file(startup_save_filepath);

    rsl::string startup_map_filepath = rex::path::join(rex::vfs::mount_path(rex::MountingPoint::Maps), rsl::format("{}.map", startup_save_file.current_map));
    m_current_map = load_map(startup_map_filepath);

    m_current_map->start_level(startup_save_file.current_level);


    //rex::memory::Blob levels_file_content = rex::vfs::read_file(startup_save_file);
    //rex::IniProcessor ini_processor = rex::IniProcessor(levels_file_content);

    //// If there are any errors processing this root levels file, we cannot start the game, therefore we assert.
    //rex::Error err = ini_processor.process();
    //REX_ASSERT_X(!err.has_error(), "failed to load levels file, game cannot boot. Please investigate {}", rex::quoted(levels_file));

    //// Go over all the levels information and store them. This will be needed later to fully load a level
    //for (const rsl::key_value<const rsl::string_view, rex::IniHeaderWithItems>& item : ini_processor.all_items())
    //{
    //  rsl::string full_ini_filepath = rex::path::join(levels_directory, item.value.get("File"));
    //  m_levels_info.emplace_back(item.value.header(), full_ini_filepath);
    //}

    //// If no levels are loaded, that'd be very weird, so we assert here in case that'd happen
    //REX_ASSERT_X(m_levels_info.size() > 0, "0 levels loaded from levels file. This is not supported as the engine doesn't know where to start the game.");
    //REX_INFO(LogGameSession, "{} levels loaded", m_levels_info.size());

    //// By default the first level in the list is loaded, however, the user has the option to provide a start level
    //rsl::string_view start_level_name = rex::cmdline::get_argument("StartLevel").value_or("");
    //auto level_info_it = m_levels_info.begin();
    //if (!start_level_name.empty())
    //{
    //  level_info_it = rsl::find_if(m_levels_info.begin(), m_levels_info.end(), 
    //    [start_level_name](const LevelInfo& levelInfo) 
    //    {
    //      return levelInfo.name() == start_level_name; 
    //    });

    //  if (level_info_it == m_levels_info.end())
    //  {
    //    REX_ERROR(LogGameSession, "Failed to find level {} in list of levels. Using first level as default.", rex::quoted(start_level_name));
    //    level_info_it = m_levels_info.begin();
    //  }
    //}

    //REX_INFO(LogGameSession, "Starting level {}", rex::quoted(level_info_it->name()));

    //// We found which level we need to start the game with, load it and start it
    //m_current_level = rsl::make_unique<Level>(*level_info_it);
    //m_current_level->start(rex::cmdline::get_argument("StartMap").value_or(""));
  }

  rsl::unique_ptr<Map> GameSession::load_map(rsl::string_view filepath)
  {
    if (!rex::vfs::is_file(filepath))
    {
      REX_ERROR(LogGameSession, "Failed to load map. Filepath doesn't exist. filepath: {}", filepath);
      return nullptr;
    }

    rex::memory::Blob file_content = rex::vfs::read_file(filepath);
    rex::json::json map_json = rex::json::parse(file_content);

    return rsl::make_unique<Map>(m_scene.get(), map_json);
  }
}
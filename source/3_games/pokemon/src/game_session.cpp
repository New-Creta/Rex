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

namespace pokemon
{
  DEFINE_LOG_CATEGORY(LogGameSession);

  GameSession::GameSession()
    : m_scene_camera(glm::vec3(0.0f, 5.0f, -20.0f), rsl::deg_angle(45.0f), static_cast<f32>(1280), static_cast<f32>(720), 0.1f, 1000.0f)
    , m_scene_viewport_width(1280)
    , m_scene_viewport_height(720)
  {
    rex::vfs::mount(rex::MountingPoint::Maps, rex::path::join(rex::vfs::project_root(), "maps"));
    m_scene = rsl::make_unique<rex::gfx::Scene>();
    m_scene_renderer = rex::gfx::add_renderer<rex::gfx::SceneRenderer>();
    m_light_direction.y = -1.0f;
    m_light_direction.z = 2.0f;

    initialize_common_resources();
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
    rsl::string startup_map_filepath = rex::path::join(rex::vfs::mount_path(rex::MountingPoint::Maps), rsl::format("{}.map", startup_save_file.current_map));
    m_current_map = load_map(startup_map_filepath);
    REX_ASSERT_X(m_current_map != nullptr, "Failed to load map");

    // Once the current map is loaded, load in the chunk the player is in
    m_current_map->load_chunk(startup_save_file.current_chunk);
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
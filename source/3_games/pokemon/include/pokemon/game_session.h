#pragma once

#include "rex_std/vector.h"
#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"

#include "pokemon/map.h"

#include "rex_renderer_core/rendering/camera.h"
#include "rex_renderer_core/rendering/scene_renderer.h"
#include "glm/glm.hpp"

namespace pokemon
{
  // A game session is an object that holds the current state of the game
  class GameSession
  {
  public:
    GameSession();

    void update();

  private:
    void initialise_world();

    rsl::unique_ptr<Map> load_map(rsl::string_view filepath);

  private:
    rsl::vector<LevelInfo> m_levels_info;
    rsl::unique_ptr<Map> m_current_map;
    
    rsl::unique_ptr<rex::gfx::Scene> m_scene;
    rex::gfx::SceneRenderer* m_scene_renderer;
    rex::gfx::Camera m_scene_camera;
    s32 m_scene_viewport_width;
    s32 m_scene_viewport_height;
    glm::vec3 m_light_direction;
  };
}
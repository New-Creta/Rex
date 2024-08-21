#pragma once

#include "rex_std/string_view.h"
#include "rex_renderer_core/scenegraph/scene.h"
#include "rex_std/unordered_map.h"
#include "pokemon/level.h"

#include "rex_renderer_core/scenegraph/entity.h"

#include "rex_engine/text_processing/json.h"
#include "rex_renderer_core/resources/sampler_2d.h"
#include "rex_renderer_core/resources/texture_2d.h"
#include "rex_renderer_core/materials/material.h"
#include "glm/glm.hpp"

namespace rex
{
  namespace gfx
  {
    class Scene;
  }
}

namespace pokemon
{
  class Chunk
  {
  public:
    Chunk(rex::gfx::Entity& entity, rsl::string_view name, rsl::string_view background, glm::vec3 pos);

  private:
    rsl::unique_ptr<rex::gfx::Material> m_material;
    rsl::unique_ptr<rex::gfx::Texture2D> m_texture;
    rsl::unique_ptr<rex::gfx::Sampler2D> m_sampler;
    glm::vec3 m_pos;
    rex::gfx::Entity m_entity;
  };

  // A map is a section for the in game level
  // a game consists of multiple levels
  // a level consists of 1 or more maps
  // for menus, the map is often empty
  // for the actual game, the map is filled with objects
  // In pokemon's case, a map is an area like Johto, Kanto, Hoenn
  class Map
  {
  public: 
    Map(rex::gfx::Scene* scene, const rex::json::json& jsonBlob);

    void start_level(rsl::string_view name);

  private:
    rsl::string m_name;
    rsl::unordered_map<rsl::string, LevelInfo> m_level_infos;
    const LevelInfo* m_current_level_info;
    rex::gfx::Scene* m_scene;

    rsl::vector<Chunk> m_chunks;
  };
}